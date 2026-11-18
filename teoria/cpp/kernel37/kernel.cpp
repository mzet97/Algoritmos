/* Nucleo da aula 37: escalonador preemptivo por timer CLINT.
 * Duas tarefas em modo-S (A e B) sob paginacao Bare; o timer em modo-M
 * interrompe, salva o frame da atual e restaura o da proxima (a troca e
 * decidida no trap_handler; o trap.S so restaura o que vier). Quando as
 * duas ja rodaram e ha trocas suficientes com o timer provado, SCHED_OK.
 * Sem syscall e sem falta de pagina nesta etapa: ja provadas na 36 e 35.
 * Marcadores: BOOT_OK TIMER_OK SCHED_OK DONE. */
#include <stdint.h>

#define UART 0x10000000UL
#define CLINT_MTIMECMP 0x2004000UL
#define CLINT_MTIME 0x200BFF8UL
#define TEST_DEVICE 0x100000UL
#define INTERVALO_TICKS 100000UL /* MTIME do virt: 10 MHz -> 10 ms */

#define F_SP 1
#define F_MEPC 31

typedef struct {
    uint64_t f[32];
} TrapFrame;

typedef struct {
    TrapFrame salvo;
    int rodou;
} Tarefa;

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

static uint8_t pilha_a[8192] __attribute__((aligned(16)));
static uint8_t pilha_b[8192] __attribute__((aligned(16)));
static Tarefa tarefas[2];
static int atual = 0;
static int lancadas = 0;

static void espera(void) { for (volatile int i = 0; i < 20000; i += 1) {} }

static void tarefa_a(void) {
    for (;;) { *uart = (uint8_t)'A'; espera(); }
}

static void tarefa_b(void) {
    for (;;) { *uart = (uint8_t)'B'; espera(); }
}

static uint64_t ticks = 0;
static int timer_ok = 0, trocas = 0;
static TrapFrame saida;

extern "C" TrapFrame *trap_handler(TrapFrame *tf) {
    uint64_t causa;
    __asm__ volatile("csrr %0, mcause" : "=r"(causa));

    if ((int64_t)causa < 0) {
        if ((causa & 0xFF) == 7) { /* timer da maquina (CLINT) */
            uint64_t agora = *(volatile uint64_t *)CLINT_MTIME;
            *(volatile uint64_t *)CLINT_MTIMECMP = agora + INTERVALO_TICKS;
            ticks += 1;
            if (ticks >= 3 && !timer_ok) { timer_ok = 1; kputs("\nTIMER_OK\n"); }
            if (lancadas) {
                tarefas[atual].salvo = *tf;
                tarefas[atual].rodou = 1;
                atual = 1 - atual;
                trocas += 1;
                if (trocas >= 10 && timer_ok && tarefas[0].rodou && tarefas[1].rodou) {
                    kputs("\nSCHED_OK\nDONE\n");
                    desligar(0x5555);
                }
                saida = tarefas[atual].salvo;
                return &saida;
            }
        }
        return tf;
    }
    kputs("TRAP_SINCRONA_INESPERADA causa=");
    kputhex(causa);
    kputc('\n');
    desligar(0x3333);
    return tf;
}

extern "C" void lancar_tarefa(TrapFrame *tf);

static void preparar_tarefa(int i, void (*entrada)(void), uint8_t *pilha) {
    for (int k = 0; k < 32; k++) tarefas[i].salvo.f[k] = 0;
    tarefas[i].salvo.f[F_SP] = (uint64_t)(pilha + 8192);
    tarefas[i].salvo.f[F_MEPC] = (uint64_t)entrada;
    tarefas[i].rodou = 0;
}

extern "C" void kmain(void) {
    kputs("BOOT_OK\n");
    __asm__ volatile("csrw pmpaddr0, %0\n csrw pmpcfg0, %1" ::"r"(-1UL), "r"(0xFUL));
    preparar_tarefa(0, tarefa_a, pilha_a);
    preparar_tarefa(1, tarefa_b, pilha_b);

    *(volatile uint64_t *)CLINT_MTIMECMP =
        *(volatile uint64_t *)CLINT_MTIME + INTERVALO_TICKS;
    __asm__ volatile("csrw mie, %0" ::"r"(1UL << 7));
    uint64_t mstatus;
    __asm__ volatile("csrr %0, mstatus" : "=r"(mstatus));
    mstatus &= ~(3UL << 11);
    mstatus |= (1UL << 11) | (1UL << 7); /* MPP=01 (modo-S), MPIE=1 */
    __asm__ volatile("csrw mstatus, %0" ::"r"(mstatus));

    atual = 0;
    lancadas = 1;
    lancar_tarefa(&tarefas[0].salvo);
    desligar(0x3333); /* nunca alcancado */
}
