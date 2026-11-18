/* Nucleo da aula 36: modo usuario e syscall sob paginacao Bare.
 * kmain abre a PMP (S/U nasce sem acesso a nada) e desce a modo-S com
 * mret (MPP=01). A tarefa prova que NAO esta em modo-M tentando ler o
 * CSR mstatus (sinal de privilegio: instrucao ilegal, causa 2) e depois
 * usa o ecall de modo-S (causa 8) como syscall: a7=1 anuncia, a7=2
 * devolve a0+1. A combinacao com o Sv39 da aula 35 e o kernel da 33.
 * Marcadores: BOOT_OK UMODE_OK SYSCALL_OK ECHO_OK DONE. */
#include <stdint.h>

#define UART 0x10000000UL
#define TEST_DEVICE 0x100000UL

#define F_A0 9
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

static void desligar(uint64_t codigo) {
    *(volatile uint32_t *)TEST_DEVICE = (uint32_t)codigo;
    for (;;) { __asm__ volatile("wfi"); }
}

static int umode_ok = 0, syscall_ok = 0, echo_ok = 0;

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
        case 2: /* instrucao ilegal: a sonda de mstatus vinda de S */
            if (!umode_ok) {
                umode_ok = 1;
                kputs("UMODE_OK\n");
                tf->f[F_MEPC] += 4;
                return tf;
            }
            kputs("ILEGAL_REPETIDA\n");
            desligar(0x3333);
            return tf;
        case 9: /* ecall de modo-S: a nossa syscall (8 seria modo-U) */
            if (tf->f[F_A7] == 1 && !syscall_ok) {
                syscall_ok = 1;
                kputs("SYSCALL_OK\n");
                tf->f[F_A0] = 0;
            } else if (tf->f[F_A7] == 2 && !echo_ok) {
                echo_ok = 1;
                kputs("ECHO_OK\n");
                tf->f[F_A0] += 1; /* eco+1: o retorno viaja no a0 */
            } else {
                kputs("SYSCALL_DESCONHECIDA a7=");
                kputhex(tf->f[F_A7]);
                kputc('\n');
                desligar(0x3333);
            }
            tf->f[F_MEPC] += 4;
            return tf;
        default:
            kputs("TRAP_INESPERADA causa=");
            kputhex(causa);
            kputc('\n');
            desligar(0x3333);
            return tf;
    }
}

/* Roda em modo-S: sonda o privilegio, depois duas syscalls. */
static void modo_s_tarefa(void) {
    uint64_t lixo = 0;
    __asm__ volatile("csrr %0, mstatus" : "=r"(lixo)); /* causa 2: UMODE_OK */
    (void)lixo;

    register uint64_t a7 __asm__("a7") = 1;
    register uint64_t a0 __asm__("a0") = 0;
    __asm__ volatile("ecall" : "+r"(a7), "+r"(a0));
    if (a0 != 0) {
        kputs("RETORNO_A7_1_DIVERGIU\n");
        desligar(0x3333);
    }

    a7 = 2;
    a0 = 41;
    __asm__ volatile("ecall" : "+r"(a7), "+r"(a0));
    if (a0 != 42) {
        kputs("ECO_DIVERGIU\n");
        desligar(0x3333);
    }
    kputs("DONE\n");
    /* Sem chamada de saida nesta etapa: desliga pelo test device. */
    *(volatile uint32_t *)TEST_DEVICE = 0x5555;
    for (;;) { __asm__ volatile("wfi"); }
}

extern "C" void kmain(void) {
    kputs("BOOT_OK\n");
    __asm__ volatile("csrw pmpaddr0, %0\n csrw pmpcfg0, %1" ::"r"(-1UL), "r"(0xFUL));
    uint64_t mstatus;
    __asm__ volatile("csrr %0, mstatus" : "=r"(mstatus));
    mstatus &= ~(3UL << 11);
    mstatus |= (1UL << 11); /* MPP=01: o proximo mret cai em modo-S */
    __asm__ volatile("csrw mstatus, %0" ::"r"(mstatus));
    __asm__ volatile("csrw mepc, %0" ::"r"((uint64_t)modo_s_tarefa));
    __asm__ volatile("mret");
    desligar(0x3333); /* nunca alcancado */
}
