/* Nucleo da aula 39: sincronizacao e execucao multicore (QEMU, -smp 2).
 * Hart 0 e hart 1 incrementam o mesmo contador N vezes cada, sob um
 * spinlock implementado com amoswap (adquire com .aq, libera com .rl).
 * Sem o lock, os incrementos leitura-modificacao-escrita se perderiam;
 * com ele, a soma final tem de ser exatamente 2*N. Qualquer outro total
 * e perda de corrida observada, nao azar: desliga com 0x3333.
 * Tudo em modo-M com paginacao Bare: o tema e a exclusao mutua entre
 * harts, nao privilegio (36) nem MMU (35).
 * Marcadores: BOOT_OK HART1_OK SMP_OK DONE. */
#include <stdint.h>

#define UART 0x10000000UL
#define TEST_DEVICE 0x100000UL

#define N_VOLTAS 500

typedef struct {
    uint64_t f[32];
} TrapFrame;

volatile int liberar_harts = 0;
volatile int hart1_ok = 0;

static volatile uint8_t *const uart = (volatile uint8_t *)UART;

static void kputc(char c) { *uart = (uint8_t)c; }
static void kputs(const char *s) { while (*s) kputc(*s++); }

/* ---------- spinlock com amoswap ---------- */
static volatile int tranca = 0;

static void adquirir(void) {
    int visto;
    do {
        __asm__ volatile("amoswap.w.aq %0, %1, %2"
                         : "=r"(visto)
                         : "r"(1), "A"(* (volatile int *)&tranca));
    } while (visto != 0);
}

static void liberar(void) {
    __asm__ volatile("amoswap.w.rl x0, x0, %0" ::"A"(* (volatile int *)&tranca));
}

static volatile uint64_t contador = 0;

static void desligar(uint64_t codigo) {
    *(volatile uint32_t *)TEST_DEVICE = (uint32_t)codigo;
    for (;;) { __asm__ volatile("wfi"); }
}

extern "C" TrapFrame *trap_handler(TrapFrame *tf) {
    (void)tf;
    kputs("TRAP_INESPERADA_NA_39\n");
    desligar(0x3333);
    return tf;
}

/* Hart 1 (modo-M, chamado pelo boot.S apos o sinal): conta N vezes. */
extern "C" void hart1_conta(void) {
    /* mscratch do hart 1: ele nao usa trap, mas o registro nao pode ficar
       com lixo do hart 0 se um dia uma interrupcao chegar. */
    __asm__ volatile("csrw mscratch, %0" ::"r"(0UL));
    for (int i = 0; i < N_VOLTAS; i++) {
        adquirir();
        contador += 1;
        liberar();
    }
    kputs("HART1_OK\n");
    hart1_ok = 1;
}

extern "C" void kmain(void) {
    kputs("BOOT_OK\n");
    liberar_harts = 1; /* solta o hart 1 */
    for (int i = 0; i < N_VOLTAS; i++) {
        adquirir();
        contador += 1;
        liberar();
    }
    /* Espera limitada pelo anuncio do hart 1 (QEMU: rapido; sem ele, o
       timeout do teste acusa em vez de pendurar). */
    for (volatile int i = 0; i < 5000000 && !hart1_ok; i += 1) {}
    if (!hart1_ok) {
        kputs("HART1_NAO_ANUNCIOU\n");
        desligar(0x3333);
    }
    if (contador != 2UL * N_VOLTAS) {
        kputs("SOMA_DIVERGIU\n");
        desligar(0x3333);
    }
    kputs("SMP_OK\nDONE\n");
    desligar(0x5555);
}
