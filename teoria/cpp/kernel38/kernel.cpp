/* Nucleo da aula 38: E/S e sistema de arquivos minimo em memoria.
 * Dois arquivos somente-leitura vivem como bytes estaticos no kernel
 * (sem disco, sem bloco, sem diretorio: nome -> (base, tamanho)).
 * Syscall a7=3, a0=id, a1=destino em S, a2=maximo: copia e devolve o
 * tamanho em a0 (-1 se o id nao existe). A tarefa le os dois arquivos,
 * ecoa o conteudo na serial e so anuncia FS_OK se os tamanhos baterem.
 * Persistencia e escrita ficam fora: e ROM didatica, nao FS real.
 * Marcadores: BOOT_OK FS_OK DONE. */
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

/* ---------- a "ROM" didatica ---------- */
static const char arq0_nome[] = "versao.txt";
static const uint8_t arq0_dados[] = "kernel38 v1\n";
static const char arq1_nome[] = "const.txt";
static const uint8_t arq1_dados[] = "PI=314159\n";

typedef struct {
    const char *nome;
    const uint8_t *dados;
    uint64_t tamanho;
} Arquivo;

static const Arquivo arquivos[] = {
    {arq0_nome, arq0_dados, sizeof(arq0_dados) - 1},
    {arq1_nome, arq1_dados, sizeof(arq1_dados) - 1},
};
#define N_ARQUIVOS (sizeof(arquivos) / sizeof(arquivos[0]))

static void desligar(uint64_t codigo) {
    *(volatile uint32_t *)TEST_DEVICE = (uint32_t)codigo;
    for (;;) { __asm__ volatile("wfi"); }
}

extern "C" TrapFrame *trap_handler(TrapFrame *tf) {
    uint64_t causa;
    __asm__ volatile("csrr %0, mcause" : "=r"(causa));

    if ((int64_t)causa < 0) {
        kputs("INTERRUPCAO_INESPERADA\n");
        desligar(0x3333);
        return tf;
    }
    if (causa == 9 && tf->f[F_A7] == 3) { /* ler arquivo */
        uint64_t id = tf->f[F_A0];
        uint8_t *dst = (uint8_t *)tf->f[F_A1];
        uint64_t max = tf->f[F_A2];
        if (id >= N_ARQUIVOS) {
            tf->f[F_A0] = (uint64_t)-1;
        } else {
            uint64_t n = arquivos[id].tamanho < max ? arquivos[id].tamanho : max;
            for (uint64_t i = 0; i < n; i++) dst[i] = arquivos[id].dados[i];
            tf->f[F_A0] = n;
        }
        tf->f[F_MEPC] += 4;
        return tf;
    }
    kputs("TRAP_INESPERADA causa=");
    kputhex(causa);
    kputc('\n');
    desligar(0x3333);
    return tf;
}

static uint64_t sys_ler(uint64_t id, uint8_t *dst, uint64_t max) {
    register uint64_t a7 __asm__("a7") = 3;
    register uint64_t a0 __asm__("a0") = id;
    register uint64_t a1 __asm__("a1") = (uint64_t)dst;
    register uint64_t a2 __asm__("a2") = max;
    __asm__ volatile("ecall" : "+r"(a7), "+r"(a0), "+r"(a1), "+r"(a2));
    return a0;
}

/* Roda em modo-S: le os dois arquivos e ecoa na serial. */
static void modo_s_tarefa(void) {
    uint8_t buf[64];
    uint64_t n0 = sys_ler(0, buf, sizeof(buf));
    if (n0 == (uint64_t)-1 || n0 != sizeof(arq0_dados) - 1) {
        kputs("ARQ0_DIVERGIU\n");
        desligar(0x3333);
    }
    for (uint64_t i = 0; i < n0; i++) kputc((char)buf[i]);

    uint64_t n1 = sys_ler(1, buf, sizeof(buf));
    if (n1 == (uint64_t)-1 || n1 != sizeof(arq1_dados) - 1) {
        kputs("ARQ1_DIVERGIU\n");
        desligar(0x3333);
    }
    for (uint64_t i = 0; i < n1; i++) kputc((char)buf[i]);

    uint64_t invalido = sys_ler(99, buf, sizeof(buf));
    if (invalido != (uint64_t)-1) { /* id inexistente tem de negar */
        kputs("ID_INVALIDO_ACEITO\n");
        desligar(0x3333);
    }
    kputs("FS_OK\nDONE\n");
    *(volatile uint32_t *)TEST_DEVICE = 0x5555;
    for (;;) { __asm__ volatile("wfi"); }
}

extern "C" void kmain(void) {
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
