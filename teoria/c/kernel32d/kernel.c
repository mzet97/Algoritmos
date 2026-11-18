/* Nucleo da aula 32d: VFS minimo sobre a base da 32a (QEMU, -smp 1).
 * Tres arquivos somente-leitura organizados em diretorios; o caminho e
 * resolvido componente a componente ("/" separa, cada nivel confere o
 * nome no diretorio pai) em vez de comparado como string pronta: e
 * isso que faz o V ser de "virtual" — o mesmo resolvedor serviria para
 * outro suporte se um dia existisse. Syscalls em modo-S: a7=3 abre
 * (a0=caminho) e devolve fd ou -1; a7=4 le (fd, destino, max) e devolve
 * o tamanho ou -1; a7=5 fecha. Sem escrita, sem montagem de outro
 * dispositivo: a tabela e estatica e o VFS e o resolvedor + fds.
 * Marcadores: BOOT_OK VFS_OK DONE. */
#include <stdint.h>

#define UART 0x10000000UL
#define TEST_DEVICE 0x100000UL

#define F_A0 9
#define F_A1 10
#define F_A2 11
#define F_A7 16
#define F_MEPC 31

typedef struct {
    uint64_t f[32];
} TrapFrame;

volatile int liberar_harts = 0;
volatile int hart1_ok = 0;

static volatile uint8_t *const uart = (volatile uint8_t *)UART;

static void kputc(char c) { *uart = (uint8_t)c; }
static void kputs(const char *s) { while (*s) kputc(*s++); }
static void kputhex(uint64_t v) {
    kputs("0x");
    for (int i = 60; i >= 0; i -= 4) {
        int d = (int)((v >> i) & 0xF);
        kputc((char)(d < 10 ? '0' + d : 'a' + d - 10));
    }
}

/* ---------- a arvore estatica ---------- */
typedef struct No {
    const char *nome;
    int dir;              /* 1 = diretorio, 0 = arquivo */
    int pai;              /* indice do diretorio pai (-1 na raiz) */
    const uint8_t *dados; /* NULL em diretorio */
    uint64_t tamanho;
} No;

static const uint8_t arq_versao[] = "kernel32d v1\n";
static const uint8_t arq_pi[] = "PI=314159\n";
static const uint8_t arq_nada[] = "";

static const No nos[] = {
    {"", 1, -1, 0, 0},              /* 0: raiz */
    {"versao.txt", 0, 0, arq_versao, sizeof arq_versao - 1}, /* 1 */
    {"dados", 1, 0, 0, 0},           /* 2: diretorio */
    {"const.txt", 0, 2, arq_pi, sizeof arq_pi - 1},          /* 3 */
    {"vazio.txt", 0, 2, arq_nada, 0},   /* 4: arquivo vazio */
};
#define N_NOS (sizeof nos / sizeof nos[0])

static void desligar(uint64_t codigo) {
    *(volatile uint32_t *)TEST_DEVICE = (uint32_t)codigo;
    for (;;) { __asm__ volatile("wfi"); }
}

/* Compara no maximo n bytes; devolve 0 se iguais no prefixo. */
static int prefixo_igual(const char *a, const char *b, uint64_t n) {
    for (uint64_t i = 0; i < n; i++)
        if (a[i] != b[i])
            return (int)(unsigned char)a[i] - (int)(unsigned char)b[i];
    return 0;
}

static uint64_t tamanho_nome(const char *s) {
    uint64_t n = 0;
    while (s[n] != '\0' && s[n] != '/') n++;
    return n;
}

/* Resolve o caminho componente a componente. Devolve o indice do no
 * ou -1. Rejeita "..", "//" vazios no meio e caminho sem "/" inicial:
 * resolvedor permissivo demais e traversal com outro nome. */
static int resolver(const char *caminho) {
    if (caminho[0] != '/')
        return -1;
    int atual = 0;
    const char *p = caminho + 1;
    if (*p == '\0')
        return 0; /* "/" e a raiz */
    for (;;) {
        while (*p == '/') p++;
        if (*p == '\0')
            return atual;
        uint64_t n = tamanho_nome(p);
        if (n == 0)
            return -1;
        if (n == 2 && p[0] == '.' && p[1] == '.')
            return -1; /* sem subida: arvore estatica nao tem pai alem da raiz */
        int achou = -1;
        for (uint64_t i = 0; i < N_NOS; i++) {
            if (nos[i].pai != atual)
                continue;
            uint64_t m = 0;
            while (nos[i].nome[m] != '\0') m++;
            if (m == n && prefixo_igual(nos[i].nome, p, n) == 0) {
                achou = (int)i;
                break;
            }
        }
        if (achou < 0)
            return -1;
        atual = achou;
        p += n;
        if (*p == '\0')
            return nos[atual].dir ? -1 : atual; /* diretorio nao abre */
    }
}

/* ---------- fds ---------- */
#define N_FD 8
static int fd_no[N_FD];
static uint64_t fd_pos[N_FD];

static int fd_alocar(int no) {
    for (int i = 0; i < N_FD; i++) {
        if (fd_no[i] < 0) {
            fd_no[i] = no;
            fd_pos[i] = 0;
            return i;
        }
    }
    return -1;
}

TrapFrame *trap_handler(TrapFrame *tf) {
    uint64_t causa;
    __asm__ volatile("csrr %0, mcause" : "=r"(causa));

    if ((int64_t)causa < 0 || causa != 9) {
        kputs("TRAP_INESPERADA causa=");
        kputhex(causa);
        kputc('\n');
        desligar(0x3333);
        return tf;
    }
    uint64_t a7 = tf->f[F_A7];
    if (a7 == 3) { /* abrir */
        int no = resolver((const char *)tf->f[F_A0]);
        tf->f[F_A0] = (uint64_t)(no < 0 ? -1 : (int64_t)fd_alocar(no));
    } else if (a7 == 4) { /* ler */
        uint64_t id = tf->f[F_A0];
        uint8_t *dst = (uint8_t *)tf->f[F_A1];
        uint64_t max = tf->f[F_A2];
        if (id >= N_FD || fd_no[id] < 0) {
            tf->f[F_A0] = (uint64_t)-1;
        } else {
            const No *no = &nos[fd_no[id]];
            uint64_t resto = no->tamanho > fd_pos[id] ? no->tamanho - fd_pos[id] : 0;
            uint64_t n = resto < max ? resto : max;
            for (uint64_t i = 0; i < n; i++)
                dst[i] = no->dados[fd_pos[id] + i];
            fd_pos[id] += n;
            tf->f[F_A0] = n;
        }
    } else if (a7 == 5) { /* fechar */
        uint64_t id = tf->f[F_A0];
        if (id >= N_FD || fd_no[id] < 0) {
            tf->f[F_A0] = (uint64_t)-1;
        } else {
            fd_no[id] = -1;
            tf->f[F_A0] = 0;
        }
    } else {
        kputs("SYSCALL_DESCONHECIDA a7=");
        kputhex(a7);
        kputc('\n');
        desligar(0x3333);
        return tf;
    }
    tf->f[F_MEPC] += 4;
    return tf;
}

static uint64_t sys_abrir(const char *caminho) {
    register uint64_t a7 __asm__("a7") = 3;
    register uint64_t a0 __asm__("a0") = (uint64_t)caminho;
    __asm__ volatile("ecall" : "+r"(a7), "+r"(a0));
    return a0;
}

static uint64_t sys_ler(uint64_t fd, uint8_t *dst, uint64_t max) {
    register uint64_t a7 __asm__("a7") = 4;
    register uint64_t a0 __asm__("a0") = fd;
    register uint64_t a1 __asm__("a1") = (uint64_t)dst;
    register uint64_t a2 __asm__("a2") = max;
    __asm__ volatile("ecall" : "+r"(a7), "+r"(a0), "+r"(a1), "+r"(a2));
    return a0;
}

static uint64_t sys_fechar(uint64_t fd) {
    register uint64_t a7 __asm__("a7") = 5;
    register uint64_t a0 __asm__("a0") = fd;
    __asm__ volatile("ecall" : "+r"(a7), "+r"(a0));
    return a0;
}

/* Roda em modo-S: abre pelos caminhos, le, fecha, e tenta o proibido. */
static void modo_s_tarefa(void) {
    uint8_t buf[64];

    uint64_t fd = sys_abrir("/versao.txt");
    if (fd == (uint64_t)-1) { kputs("ABRIR_VERSAO_FALHOU\n"); desligar(0x3333); }
    uint64_t n = sys_ler(fd, buf, sizeof buf);
    if (n != sizeof arq_versao - 1) { kputs("LER_VERSAO_DIVERGIU\n"); desligar(0x3333); }
    for (uint64_t i = 0; i < n; i++) kputc((char)buf[i]);
    if (sys_fechar(fd) != 0) { kputs("FECHAR_FALHOU\n"); desligar(0x3333); }

    fd = sys_abrir("/dados/const.txt"); /* caminho aninhado: anda 2 niveis */
    if (fd == (uint64_t)-1) { kputs("ABRIR_ANINHADO_FALHOU\n"); desligar(0x3333); }
    n = sys_ler(fd, buf, sizeof buf);
    if (n != sizeof arq_pi - 1) { kputs("LER_ANINHADO_DIVERGIU\n"); desligar(0x3333); }
    for (uint64_t i = 0; i < n; i++) kputc((char)buf[i]);

    fd = sys_abrir("/dados/vazio.txt");
    if (fd == (uint64_t)-1) { kputs("ABRIR_VAZIO_FALHOU\n"); desligar(0x3333); }
    if (sys_ler(fd, buf, sizeof buf) != 0) { kputs("VAZIO_NAO_E_ZERO\n"); desligar(0x3333); }

    /* O proibido: diretorio nao abre, ausente nega, ".." nao sobe. */
    if (sys_abrir("/dados") != (uint64_t)-1) { kputs("DIR_ABRIU\n"); desligar(0x3333); }
    if (sys_abrir("/nao/existe.txt") != (uint64_t)-1) { kputs("AUSENTE_ABRIU\n"); desligar(0x3333); }
    if (sys_abrir("/dados/../versao.txt") != (uint64_t)-1) { kputs("DOTDOT_PASSOU\n"); desligar(0x3333); }
    if (sys_ler(99, buf, sizeof buf) != (uint64_t)-1) { kputs("FD_INVALIDO_LEU\n"); desligar(0x3333); }

    kputs("VFS_OK\nDONE\n");
    *(volatile uint32_t *)TEST_DEVICE = 0x5555;
    for (;;) { __asm__ volatile("wfi"); }
}

void kmain(void) {
    for (int i = 0; i < N_FD; i++) fd_no[i] = -1;
    kputs("BOOT_OK\n");
    __asm__ volatile("csrw pmpaddr0, %0\n csrw pmpcfg0, %1" ::"r"(-1UL), "r"(0xFUL));
    uint64_t mstatus;
    __asm__ volatile("csrr %0, mstatus" : "=r"(mstatus));
    mstatus &= ~(3UL << 11);
    mstatus |= (1UL << 11);
    __asm__ volatile("csrw mstatus, %0" ::"r"(mstatus));
    __asm__ volatile("csrw mepc, %0" ::"r"((uint64_t)modo_s_tarefa));
    __asm__ volatile("mret");
    desligar(0x3333); /* nunca alcancado */
}
