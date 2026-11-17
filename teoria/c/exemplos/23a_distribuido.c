/* Aula 23a - dois nos chave-valor com log duravel, idempotencia e falhas.
 * Modos:
 *   ./prog                 executa o roteiro de falhas (checar)
 *   ./prog checar          idem
 *   ./prog no <porta> <log> <porta_par> <atraso_ms>
 * Protocolo (linhas, loopback apenas):
 *   PUT <id> <chave> <valor> [F]   F = encaminhado (nao reencaminha)
 *   GET <chave>
 * Respostas: OK <valor> | OK <valor> REPETIDO | NAO_ENCONTRADO | ERR <motivo>
 */
#define _POSIX_C_SOURCE 200809L
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define LINHA_MAX 512
#define ID_MAX 31
#define CHAVE_MAX 63
#define VALOR_MAX 127

typedef struct {
    char id[ID_MAX + 1];
    char chave[CHAVE_MAX + 1];
    char valor[VALOR_MAX + 1];
} Registro;

typedef struct {
    Registro *regs;
    size_t n;
    size_t cap;
    FILE *log_fp;
    int porta_par;
    long atraso_ms;
    pthread_mutex_t mu;
} No;

static No g_no;

static int token_valido(const char *s, size_t max) {
    size_t n = strlen(s);
    if (n == 0 || n > max) return 0;
    for (size_t i = 0; i < n; i++) {
        char c = s[i];
        int ok = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
                 (c >= '0' && c <= '9') || c == '.' || c == '_' || c == '-';
        if (!ok) return 0;
    }
    return 1;
}

/* Divide em tokens por espaco. Devolve numero ou -1 se exceder max_tok. */
static int dividir(char *linha, char *tok[], int max_tok) {
    int n = 0;
    char *save = NULL;
    char *t = strtok_r(linha, " \t\r\n", &save);
    while (t != NULL) {
        if (n >= max_tok) return -1;
        tok[n++] = t;
        t = strtok_r(NULL, " \t\r\n", &save);
    }
    return n;
}

static Registro *buscar_id(No *no, const char *id) {
    for (size_t i = 0; i < no->n; i++)
        if (strcmp(no->regs[i].id, id) == 0) return &no->regs[i];
    return NULL;
}

static const char *buscar_chave(No *no, const char *chave) {
    for (size_t i = no->n; i-- > 0;)
        if (strcmp(no->regs[i].chave, chave) == 0) return no->regs[i].valor;
    return NULL;
}

static int regs_acrescentar(No *no, const Registro *r) {
    if (no->n == no->cap) {
        if (no->cap > SIZE_MAX / 2 / sizeof(Registro)) return 0;
        size_t nova = no->cap ? no->cap * 2 : 64;
        Registro *p = realloc(no->regs, nova * sizeof *p);
        if (!p) return 0;
        no->regs = p;
        no->cap = nova;
    }
    no->regs[no->n++] = *r;
    return 1;
}

/* Replay: relê o log; linha malformada (ex.: cauda rasgada) e ignorada. */
static void replay(No *no, const char *caminho) {
    FILE *f = fopen(caminho, "r");
    if (!f) return;
    char *linha = NULL;
    size_t cap = 0;
    ssize_t len;
    while ((len = getline(&linha, &cap, f)) != -1) {
        char copia[LINHA_MAX + 1];
        if (len > LINHA_MAX) continue;
        memcpy(copia, linha, (size_t)len);
        copia[len] = '\0';
        char *tok[4];
        if (dividir(copia, tok, 4) != 3) continue;
        if (!token_valido(tok[0], ID_MAX) || !token_valido(tok[1], CHAVE_MAX) ||
            !token_valido(tok[2], VALOR_MAX)) continue;
        Registro r;
        snprintf(r.id, sizeof r.id, "%s", tok[0]);
        snprintf(r.chave, sizeof r.chave, "%s", tok[1]);
        snprintf(r.valor, sizeof r.valor, "%s", tok[2]);
        if (buscar_id(no, r.id) == NULL) (void)regs_acrescentar(no, &r);
    }
    free(linha);
    fclose(f);
}

/* Persiste com fsync ANTES de aplicar/responder. Chamado sob o mutex. */
static int persistir(No *no, const Registro *r) {
    if (fprintf(no->log_fp, "%s %s %s\n", r->id, r->chave, r->valor) < 0) return 0;
    if (fflush(no->log_fp) != 0) return 0;
    if (fsync(fileno(no->log_fp)) != 0) return 0;
    return 1;
}

static int enviar_tudo(int fd, const char *buf, size_t n) {
    size_t off = 0;
    while (off < n) {
        ssize_t w = send(fd, buf + off, n - off, 0);
        if (w < 0) {
            if (errno == EINTR) continue;
            return 0;
        }
        off += (size_t)w;
    }
    return 1;
}

/* Le uma linha. 1=ok, 0=EOF limpo, -1=truncado/longo/erro. */
static int ler_linha(int fd, char *buf, size_t cap) {
    size_t n = 0;
    for (;;) {
        char c;
        ssize_t r = recv(fd, &c, 1, 0);
        if (r == 0) return n == 0 ? 0 : -1;
        if (r < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (c == '\n') { buf[n] = '\0'; return 1; }
        if (n + 1 >= cap) return -1;
        buf[n++] = c;
    }
}

static void responder(int fd, const char *msg) {
    char buf[LINHA_MAX + 2];
    int len = snprintf(buf, sizeof buf, "%s\n", msg);
    if (len > 0) (void)enviar_tudo(fd, buf, (size_t)len);
}

static int conectar_timeout(int porta, int timeout_ms) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;
    int flags = fcntl(fd, F_GETFL, 0);
    (void)fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)porta);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    int rc = connect(fd, (struct sockaddr *)&addr, sizeof addr);
    if (rc != 0) {
        if (errno != EINPROGRESS) { close(fd); return -1; }
        struct pollfd pfd = {.fd = fd, .events = POLLOUT};
        if (poll(&pfd, 1, timeout_ms) <= 0) { close(fd); return -1; }
        int erro = 0;
        socklen_t elen = sizeof erro;
        if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &erro, &elen) != 0 || erro != 0) {
            close(fd); return -1;
        }
    }
    (void)fcntl(fd, F_SETFL, flags);
    struct timeval tv = {.tv_sec = 2, .tv_usec = 0};
    (void)setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);
    (void)setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof tv);
    return fd;
}

/* Melhor esforco, fora do mutex, com timeout: par caido degrada, nao trava. */
static void encaminhar(No *no, const Registro *r) {
    if (no->porta_par == 0) return;
    if (no->atraso_ms > 0) {
        struct timespec ts = {.tv_sec = no->atraso_ms / 1000,
                              .tv_nsec = (no->atraso_ms % 1000) * 1000000L};
        (void)nanosleep(&ts, NULL);
    }
    int fd = conectar_timeout(no->porta_par, 2000);
    if (fd < 0) return;
    char linha[LINHA_MAX + 2];
    int len = snprintf(linha, sizeof linha, "PUT %s %s %s F\n", r->id, r->chave, r->valor);
    if (len > 0 && enviar_tudo(fd, linha, (size_t)len)) {
        char resp[LINHA_MAX + 1];
        (void)ler_linha(fd, resp, sizeof resp);
    }
    close(fd);
}

static void tratar(int fd, char *linha) {
    char *tok[6];
    int n = dividir(linha, tok, 6);
    if (n <= 0) { responder(fd, "ERR pedido_vazio"); return; }
    if (strcmp(tok[0], "GET") == 0) {
        if (n != 2 || !token_valido(tok[1], CHAVE_MAX)) { responder(fd, "ERR get_invalido"); return; }
        pthread_mutex_lock(&g_no.mu);
        const char *v = buscar_chave(&g_no, tok[1]);
        char resp[LINHA_MAX];
        if (v) snprintf(resp, sizeof resp, "OK %s", v);
        else snprintf(resp, sizeof resp, "NAO_ENCONTRADO");
        pthread_mutex_unlock(&g_no.mu);
        responder(fd, resp);
        return;
    }
    if (strcmp(tok[0], "PUT") == 0) {
        int encaminhado = (n == 5 && strcmp(tok[4], "F") == 0);
        if ((n != 4 && !encaminhado) || !token_valido(tok[1], ID_MAX) ||
            !token_valido(tok[2], CHAVE_MAX) || !token_valido(tok[3], VALOR_MAX)) {
            responder(fd, "ERR put_invalido");
            return;
        }
        Registro r;
        snprintf(r.id, sizeof r.id, "%s", tok[1]);
        snprintf(r.chave, sizeof r.chave, "%s", tok[2]);
        snprintf(r.valor, sizeof r.valor, "%s", tok[3]);
        char resp[LINHA_MAX];
        int repetido = 0;
        pthread_mutex_lock(&g_no.mu);
        Registro *antigo = buscar_id(&g_no, r.id);
        if (antigo) {
            /* Idempotencia: devolve o primeiro resultado, sem reaplicar. */
            snprintf(resp, sizeof resp, "OK %s REPETIDO", antigo->valor);
            repetido = 1;
        } else if (!persistir(&g_no, &r) || !regs_acrescentar(&g_no, &r)) {
            snprintf(resp, sizeof resp, "ERR log");
        } else {
            snprintf(resp, sizeof resp, "OK %s", r.valor);
        }
        pthread_mutex_unlock(&g_no.mu);
        /* Encaminha antes de responder (so PUT novo, fora do mutex):
           quando o cliente recebe OK, a replica ja recebeu ou degradou. */
        if (!repetido && resp[0] == 'O' && !encaminhado) encaminhar(&g_no, &r);
        responder(fd, resp);
        return;
    }
    responder(fd, "ERR op_desconhecida");
}

static void *atende(void *arg) {
    int fd = (int)(intptr_t)arg;
    struct timeval tv = {.tv_sec = 10, .tv_usec = 0};
    (void)setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);
    char linha[LINHA_MAX + 1];
    for (;;) {
        int rc = ler_linha(fd, linha, sizeof linha);
        if (rc == 0) break;       /* EOF limpo */
        if (rc < 0) break;        /* truncado/longo: fecha so esta conexao */
        tratar(fd, linha);
    }
    close(fd);
    return NULL;
}

static int modo_no(int porta, const char *log_caminho, int porta_par, long atraso_ms) {
    memset(&g_no, 0, sizeof g_no);
    g_no.porta_par = porta_par;
    g_no.atraso_ms = atraso_ms;
    (void)pthread_mutex_init(&g_no.mu, NULL);
    replay(&g_no, log_caminho);
    g_no.log_fp = fopen(log_caminho, "a");
    if (!g_no.log_fp) { perror("log"); return EXIT_FAILURE; }

    int srv = socket(AF_INET, SOCK_STREAM, 0);
    if (srv < 0) { perror("socket"); return EXIT_FAILURE; }
    int um = 1;
    (void)setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &um, sizeof um);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)porta);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(srv, (struct sockaddr *)&addr, sizeof addr) != 0) { perror("bind"); return EXIT_FAILURE; }
    if (listen(srv, 32) != 0) { perror("listen"); return EXIT_FAILURE; }
    for (;;) {
        int fd = accept(srv, NULL, NULL);
        if (fd < 0) {
            if (errno == EINTR) continue;
            return EXIT_FAILURE;
        }
        pthread_t th;
        if (pthread_create(&th, NULL, atende, (void *)(intptr_t)fd) == 0)
            (void)pthread_detach(th);
        else
            close(fd);
    }
}

/* ---------- roteiro de falhas (modo checar) ---------- */
static pid_t filhos[4];
static int nfilhos = 0;
static int testes = 0;

static void encerrar_filhos(void) {
    for (int i = 0; i < nfilhos; i++)
        if (filhos[i] > 0) (void)kill(filhos[i], SIGKILL);
    for (int i = 0; i < nfilhos; i++)
        if (filhos[i] > 0) (void)waitpid(filhos[i], NULL, 0);
}

static void falha(const char *msg) {
    fprintf(stderr, "FALHA: %s\n", msg);
    encerrar_filhos();
    exit(EXIT_FAILURE);
}

static void checa(int cond, const char *msg) {
    if (!cond) falha(msg);
    testes++;
    printf("ok %d - %s\n", testes, msg);
}

static int porta_livre(void) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) falha("socket porta_livre");
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(fd, (struct sockaddr *)&addr, sizeof addr) != 0) falha("bind porta_livre");
    socklen_t len = sizeof addr;
    if (getsockname(fd, (struct sockaddr *)&addr, &len) != 0) falha("getsockname");
    int porta = ntohs(addr.sin_port);
    close(fd);
    return porta;
}

static pid_t iniciar_no(const char *exe, int porta, const char *log, int par, long atraso) {
    pid_t pid = fork();
    if (pid < 0) falha("fork");
    if (pid == 0) {
        char p[16], pp[16], at[16];
        snprintf(p, sizeof p, "%d", porta);
        snprintf(pp, sizeof pp, "%d", par);
        snprintf(at, sizeof at, "%ld", atraso);
        execl(exe, exe, "no", p, log, pp, at, (char *)NULL);
        _exit(127);
    }
    filhos[nfilhos++] = pid;
    /* Espera aceitar conexoes (ou morrer). */
    for (int i = 0; i < 100; i++) {
        int st;
        if (waitpid(pid, &st, WNOHANG) == pid) falha("no morreu ao iniciar");
        int fd = conectar_timeout(porta, 200);
        if (fd >= 0) { close(fd); return pid; }
        struct timespec ts = {.tv_sec = 0, .tv_nsec = 50000000L};
        (void)nanosleep(&ts, NULL);
    }
    falha("no nao aceitou a tempo");
    return -1;
}

static void matar(pid_t pid) {
    (void)kill(pid, SIGKILL);
    (void)waitpid(pid, NULL, 0);
    for (int i = 0; i < nfilhos; i++)
        if (filhos[i] == pid) filhos[i] = -1;
}

/* Envia uma linha e le a resposta. Devolve 1 se houve resposta. */
static int cliente(int porta, const char *linha, char *resp, size_t cap) {
    int fd = conectar_timeout(porta, 2000);
    if (fd < 0) return 0;
    char buf[LINHA_MAX + 2];
    int len = snprintf(buf, sizeof buf, "%s\n", linha);
    if (len <= 0 || !enviar_tudo(fd, buf, (size_t)len)) { close(fd); return 0; }
    int rc = ler_linha(fd, resp, cap);
    close(fd);
    return rc == 1;
}

static double agora_s(void) {
    struct timespec ts;
    (void)clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

static int modo_checar(const char *exe) {
    char dir[] = "/tmp/c23a_XXXXXX";
    if (!mkdtemp(dir)) { perror("mkdtemp"); return EXIT_FAILURE; }
    char logA[PATH_MAX], logB[PATH_MAX];
    snprintf(logA, sizeof logA, "%s/a.log", dir);
    snprintf(logB, sizeof logB, "%s/b.log", dir);
    int portaA = porta_livre();
    int portaB = porta_livre();
    char resp[LINHA_MAX + 1];

    pid_t noA = iniciar_no(exe, portaA, logA, portaB, 0);
    pid_t noB = iniciar_no(exe, portaB, logB, portaA, 0);

    checa(cliente(portaA, "PUT id1 k1 v1", resp, sizeof resp) && strcmp(resp, "OK v1") == 0,
          "PUT inicial responde OK");
    checa(cliente(portaB, "GET k1", resp, sizeof resp) && strcmp(resp, "OK v1") == 0,
          "replica recebe via encaminhamento");
    checa(cliente(portaA, "PUT id1 k1 vDIFERENTE", resp, sizeof resp) &&
              strcmp(resp, "OK v1 REPETIDO") == 0,
          "duplicata com valor diferente devolve a primeira (REPETIDO)");
    checa(cliente(portaB, "GET k1", resp, sizeof resp) && strcmp(resp, "OK v1") == 0,
          "duplicata nao duplica efeito na replica");
    checa(cliente(portaA, "PUT idRuim k1", resp, sizeof resp) && strncmp(resp, "ERR", 3) == 0,
          "pedido malformado recebe ERR");
    checa(cliente(portaA, "GET k1", resp, sizeof resp) && strcmp(resp, "OK v1") == 0,
          "servidor sobrevive a pedido malformado");

    /* Truncado: bytes parciais e fecha; so esta conexao morre. */
    {
        int fd = conectar_timeout(portaA, 2000);
        checa(fd >= 0, "conecta para truncar");
        (void)enviar_tudo(fd, "PUT idPar", 9);
        close(fd);
    }
    checa(cliente(portaA, "GET k1", resp, sizeof resp) && strcmp(resp, "OK v1") == 0,
          "servidor sobrevive a linha truncada");

    matar(noB);
    checa(cliente(portaA, "PUT id2 k2 v2", resp, sizeof resp) && strcmp(resp, "OK v2") == 0,
          "escrita com replica caida responde (degradado)");

    /* Cauda rasgada no log de B: replay deve ignora-la. */
    {
        FILE *f = fopen(logB, "a");
        checa(f != NULL, "abre log de B para rasgar a cauda");
        if (f) { fputs("idRasgado chaveRasgada", f); fclose(f); }
    }
    noB = iniciar_no(exe, portaB, logB, portaA, 0);
    checa(cliente(portaB, "GET k1", resp, sizeof resp) && strcmp(resp, "OK v1") == 0,
          "reinicio faz replay e ignora cauda rasgada");
    checa(cliente(portaB, "GET k2", resp, sizeof resp) && strcmp(resp, "NAO_ENCONTRADO") == 0,
          "escrita da queda ainda nao esta na replica");
    checa(cliente(portaA, "PUT id2 k2 v2", resp, sizeof resp) &&
              strcmp(resp, "OK v2 REPETIDO") == 0,
          "reenvio com mesmo id e REPETIDO na origem");
    checa(cliente(portaB, "PUT id2 k2 v2 F", resp, sizeof resp) && strcmp(resp, "OK v2") == 0,
          "reenvio dirigido entrega a escrita perdida");
    checa(cliente(portaB, "GET k2", resp, sizeof resp) && strcmp(resp, "OK v2") == 0,
          "estados convergem apos reenvio");

    /* Atraso limitado: A renasce com 400 ms de atraso no encaminhamento. */
    matar(noA);
    noA = iniciar_no(exe, portaA, logA, portaB, 400);
    double t0 = agora_s();
    int ok = cliente(portaA, "PUT id3 k3 v3", resp, sizeof resp);
    double dt = agora_s() - t0;
    checa(ok && strcmp(resp, "OK v3") == 0 && dt >= 0.35 && dt < 5.0,
          "atraso de 400 ms atrasa mas nao trava o cliente");
    checa(cliente(portaB, "GET k3", resp, sizeof resp) && strcmp(resp, "OK v3") == 0,
          "escrita atrasada chega a replica");

    matar(noA);
    matar(noB);
    (void)unlink(logA);
    (void)unlink(logB);
    (void)rmdir(dir);
    printf("DISTRIBUIDO_OK checks=%d\n", testes);
    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    (void)signal(SIGPIPE, SIG_IGN);
    if (argc >= 2 && strcmp(argv[1], "no") == 0) {
        if (argc != 6) {
            fprintf(stderr, "uso: %s no <porta> <log> <porta_par> <atraso_ms>\n", argv[0]);
            return EXIT_FAILURE;
        }
        return modo_no(atoi(argv[2]), argv[3], atoi(argv[4]), atol(argv[5]));
    }
    if (argc == 1 || (argc == 2 && strcmp(argv[1], "checar") == 0))
        return modo_checar(argv[0]);
    fprintf(stderr, "uso: %s [checar]\n", argv[0]);
    return EXIT_FAILURE;
}
