/* Nucleo da aula 34: traps em modo-M (QEMU virt, -smp 1, sem paginacao).
 * kmain instala o vetor (feito no boot.S), provoca um ecall (causa 11) e
 * uma instrucao ilegal de 32 bits (causa 2, .word 0xFFFFFFFF) e desliga
 * com 0x5555. Qualquer outra causa desliga com 0x3333 e diz qual foi:
 * trap inesperada aqui e defeito do experimento, nao do silicio.
 * Marcadores: BOOT_OK TRAP_ECALL_OK TRAP_ILLEGAL_OK DONE. */
#include <stdint.h>

#define UART 0x10000000UL
#define TEST_DEVICE 0x100000UL

#define F_MEPC 31

typedef struct {
    uint64_t f[32]; /* f[0]=x1, f[1]=sp, f[2]=x3 ... f[30]=x31, f[31]=mepc */
} TrapFrame;

/* Exigidos pelo boot.S compartilhado (hart secundario); com -smp 1
   esse caminho nunca executa, mas o simbolo precisa existir no link. */
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

static void desligar(uint64_t codigo) {
    *(volatile uint32_t *)TEST_DEVICE = (uint32_t)codigo;
    for (;;) { __asm__ volatile("wfi"); }
}

static int ecall_ok = 0;

extern "C" TrapFrame *trap_handler(TrapFrame *tf) {
    uint64_t causa, tval;
    __asm__ volatile("csrr %0, mcause" : "=r"(causa));
    __asm__ volatile("csrr %0, mtval" : "=r"(tval));

    if ((int64_t)causa < 0) {
        kputs("INTERRUPCAO_INESPERADA\n");
        desligar(0x3333);
        return tf;
    }
    switch (causa) {
        case 11: /* ecall vindo de modo-M: o nosso "int 3" didatico */
            if (!ecall_ok) { ecall_ok = 1; kputs("TRAP_ECALL_OK\n"); }
            tf->f[F_MEPC] += 4;
            return tf;
        case 2: /* instrucao ilegal: 0xFFFFFFFF nao decodifica em nada */
            kputs("TRAP_ILLEGAL_OK mtval=");
            kputhex(tval);
            kputs("\nDONE\n");
            desligar(0x5555);
            return tf;
        default:
            kputs("TRAP_INESPERADA causa=");
            kputhex(causa);
            kputs(" mtval=");
            kputhex(tval);
            kputc('\n');
            desligar(0x3333);
            return tf;
    }
}

extern "C" void kmain(void) {
    kputs("BOOT_OK\n");
    __asm__ volatile("ecall");            /* causa 11: tratada e retomada */
    __asm__ volatile(".word 0xFFFFFFFF"); /* causa 2: tratada e DONE */
    desligar(0x3333); /* nunca alcancado: a ilegal desliga antes */
}
