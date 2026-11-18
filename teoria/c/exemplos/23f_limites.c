/* Aula 23f — limites globais medidos sobre a base da 23b e da 31.
 *
 * Servidor ECHO por linhas (poll, sem threads) com tres tetos globais:
 * (1) conexoes simultaneas, (2) conexoes por IP, (3) bytes recebidos por
 * janela de 1 s. Cliente lento ocupa slot sem falar e nao paralisa os
 * demais; quem estoura teto cai com mensagem e sem vazar o slot.
 *
 * Compilar: gcc -std=c11 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/23f_limites.c -o /tmp/23f
 * Executar: /tmp/23f PORTA  (o teste python sobe, exercita e derruba)
 */
#define _POSIX_C_SOURCE 200809L
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

enum {
    MAX_CLIENTS = 32,
    TETO_CONEXOES = 8,
    TETO_POR_IP = 2,
    TETO_BYTES_JANELA = 65536,
    LINE_CAP = 1024,
    OUT_CAP = 4096
};

typedef struct {
    int fd;
    uint32_t ip;
    int surda; /* estourou bytes: le-e-descarta ate a proxima janela */
    char line[LINE_CAP];
    size_t line_len;
    char out[OUT_CAP];
    size_t out_pos;
    size_t out_len;
} Client;

static Client clientes[MAX_CLIENTS];
static int n_conexoes = 0;
static size_t bytes_janela = 0;
static uint64_t janela_ini_ms = 0;

static uint64_t agora_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

/* Nova janela de 1 s: zera o contador e dessilencia todo mundo. */
static void nova_janela(uint64_t agora)
{
    janela_ini_ms = agora;
    bytes_janela = 0;
    for (int i = 0; i < MAX_CLIENTS; i++)
        clientes[i].surda = 0;
}

static void talvez_nova_janela(uint64_t agora)
{
    if (agora - janela_ini_ms >= 1000)
        nova_janela(agora);
}

static int por_ip(uint32_t ip)
{
    int n = 0;
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clientes[i].fd >= 0 && clientes[i].ip == ip)
            n++;
    return n;
}

static void fecha(int i)
{
    if (clientes[i].fd < 0)
        return; /* ja fechado: nao conta duas vezes */
    close(clientes[i].fd);
    clientes[i].fd = -1;
    n_conexoes--;
}

static void recusa(int fd, const char *motivo)
{
    dprintf(fd, "CHEIO %s\n", motivo);
    close(fd);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "uso: %s PORTA\n", argv[0]);
        return 2;
    }
    for (int i = 0; i < MAX_CLIENTS; i++)
        clientes[i].fd = -1;

    int srv = socket(AF_INET, SOCK_STREAM, 0);
    if (srv < 0)
        return 1;
    int um = 1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &um, sizeof um);
    struct sockaddr_in end;
    memset(&end, 0, sizeof end);
    end.sin_family = AF_INET;
    end.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    end.sin_port = htons((uint16_t)atoi(argv[1]));
    if (bind(srv, (struct sockaddr *)&end, sizeof end) < 0 || listen(srv, 16) < 0)
        return 1;
    {
        int fl = fcntl(srv, F_GETFL, 0);
        if (fl < 0 || fcntl(srv, F_SETFL, fl | O_NONBLOCK) < 0)
            return 1;
    }

    janela_ini_ms = agora_ms();
    for (;;) {
        struct pollfd pf[MAX_CLIENTS + 1];
        pf[0].fd = srv;
        pf[0].events = POLLIN;
        int nslots = 0;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clientes[i].fd < 0)
                continue;
            pf[1 + nslots].fd = clientes[i].fd;
            pf[1 + nslots].events = POLLIN;
            if (clientes[i].out_len > clientes[i].out_pos)
                pf[1 + nslots].events |= POLLOUT;
            nslots++;
        }
        if (poll(pf, (nfds_t)(nslots + 1), 100) < 0) {
            if (errno == EINTR)
                continue;
            return 1;
        }
        int aceitou = 0;
        if (pf[0].revents & POLLIN) {
            /* Drena o backlog inteiro: um accept por iteracao atrasa
               a contagem sob rajada e os tetos passam a mentir. */
            for (;;) {
                struct sockaddr_in de;
                socklen_t n = sizeof de;
                int fd = accept(srv, (struct sockaddr *)&de, &n);
                if (fd < 0)
                    break;
                aceitou = 1;
                uint32_t ip = de.sin_addr.s_addr;
                talvez_nova_janela(agora_ms());
                if (n_conexoes >= TETO_CONEXOES)
                    recusa(fd, "global");
                else if (por_ip(ip) >= TETO_POR_IP)
                    recusa(fd, "por-ip");
                else {
                    int slot = -1;
                    for (int i = 0; i < MAX_CLIENTS; i++)
                        if (clientes[i].fd < 0) {
                            slot = i;
                            break;
                        }
                    if (slot < 0) {
                        recusa(fd, "slots");
                    } else {
                        clientes[slot].fd = fd;
                        clientes[slot].ip = ip;
                        clientes[slot].surda = 0;
                        clientes[slot].line_len = 0;
                        clientes[slot].out_pos = clientes[slot].out_len = 0;
                        n_conexoes++;
                    }
                }
            }
            /* Volta ao poll antes de tocar nos clientes: o pf desta
               iteracao nao contem os recem-aceitos e o revents deles
               seria lixo de pilha (fecha espurio no nascimento). */
            if (aceitou)
                continue;
        }
        int idx = 0;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clientes[i].fd < 0)
                continue;
            idx++;
            short rev = pf[idx].revents;
            if (rev & (POLLERR | POLLHUP | POLLNVAL)) {
                fecha(i);
                continue;
            }
            if (rev & POLLOUT) {
                ssize_t w = write(clientes[i].fd, clientes[i].out + clientes[i].out_pos,
                                  clientes[i].out_len - clientes[i].out_pos);
                if (w <= 0) {
                    fecha(i);
                    continue;
                }
                clientes[i].out_pos += (size_t)w;
                if (clientes[i].out_pos == clientes[i].out_len)
                    clientes[i].out_pos = clientes[i].out_len = 0;
            }
            if (rev & POLLIN) {
                talvez_nova_janela(agora_ms());
                char tmp[4096];
                ssize_t r = read(clientes[i].fd, tmp, sizeof tmp);
                if (r <= 0) {
                    fecha(i);
                    continue;
                }
                if (clientes[i].surda)
                    continue; /* silenciada: le-e-descarta ate a janela virar */
                bytes_janela += (size_t)r;
                if (bytes_janela > TETO_BYTES_JANELA) {
                    /* Nao fecha: fechar com rajada nao lida gera RST e o
                       cliente nunca leria o motivo. Silencia ate a
                       proxima janela e avisa no proprio canal. */
                    const char *msg = "CHEIO bytes\n";
                    size_t m = strlen(msg);
                    memcpy(clientes[i].out, msg, m);
                    clientes[i].out_pos = 0;
                    clientes[i].out_len = m;
                    clientes[i].surda = 1;
                    clientes[i].line_len = 0;
                    continue;
                }
                for (ssize_t k = 0; k < r; k++) {
                    if (clientes[i].line_len < LINE_CAP - 1)
                        clientes[i].line[clientes[i].line_len++] = tmp[k];
                    if (tmp[k] == '\n') {
                        clientes[i].line[clientes[i].line_len] = '\0';
                        char resp[OUT_CAP];
                        int n = snprintf(resp, sizeof resp, "ECO %s", clientes[i].line);
                        size_t off = clientes[i].out_len;
                        if (off == 0)
                            clientes[i].out_pos = 0;
                        if ((size_t)n < sizeof clientes[i].out - off) {
                            memcpy(clientes[i].out + off, resp, (size_t)n);
                            clientes[i].out_len = off + (size_t)n;
                        }
                        clientes[i].line_len = 0;
                    }
                }
                if (clientes[i].line_len == LINE_CAP - 1 && memchr(clientes[i].line, '\n', clientes[i].line_len) == NULL) {
                    /* Linha sem fim dentro do teto: erro so desta conexao. */
                    const char *msg = "ERRO linha-longa\n";
                    size_t m = strlen(msg);
                    memcpy(clientes[i].out, msg, m);
                    clientes[i].out_pos = 0;
                    clientes[i].out_len = m;
                    clientes[i].line_len = 0;
                }
            }
        }
    }
}
