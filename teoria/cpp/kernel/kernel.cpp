/* Kernel didatico RISC-V em C++ freestanding (QEMU virt, modo-M):
   o mesmo percurso da aula 32a de C, compilado como C++20 sem excecoes
   e sem RTTI. kmain e trap_handler sao extern "C" porque quem os chama
   e o assembly; todo o resto e C++ com donos explicitos onde ha dono.
 * boot -> Sv39 (gigapages) -> cai para modo-S em duas tarefas ->
 * timer CLINT preempta e alterna as tarefas -> ecall vira syscall ->
 * falta de pagina sob demanda mapeia e reexecuta -> hart 1 anuncia.
 * Marcadores na serial: BOOT_OK HART1_OK TIMER_OK SYSCALL_OK FAULT_OK
 * SCHED_OK DONE. Desligamento pelo test device (0x5555 = sucesso). */
#include <stdint.h>

#define UART 0x10000000UL
#define CLINT_MTIMECMP 0x2004000UL
#define CLINT_MTIME 0x200BFF8UL
#define TEST_DEVICE 0x100000UL
#define INTERVALO_TICKS 100000UL /* MTIME do virt: 10 MHz -> 10 ms */

#define F_SP 1
#define F_A0 9
#define F_A7 16
#define F_MEPC 31

typedef struct {
    uint64_t f[32]; /* f[0]=x1, f[1]=sp, f[2]=x3 ... f[30]=x31, f[31]=mepc */
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

/* ---------- Sv39: tabela raiz com gigapages (1 GiB) ---------- */
static uint64_t tabela_raiz[512] __attribute__((aligned(4096)));
#define PTE_V (1UL << 0)
#define PTE_R (1UL << 1)
#define PTE_W (1UL << 2)
#define PTE_X (1UL << 3)
#define PTE_AD ((1UL << 6) | (1UL << 7))

static void mapear_giga(uint64_t va, uint64_t pa, uint64_t flags) {
    tabela_raiz[(va >> 30) & 0x1FF] = ((pa >> 12) << 10) | flags | PTE_V | PTE_AD;
}

#define VA_FALTA 0x100000000UL /* 4 GiB: propositalmente sem mapa no boot */

static void montar_paginacao(void) {
    mapear_giga(0x00000000UL, 0x00000000UL, PTE_R | PTE_W);          /* UART e CLINT */
    mapear_giga(0x80000000UL, 0x80000000UL, PTE_R | PTE_W | PTE_X);  /* RAM (codigo e dados) */
    uint64_t satp = (8UL << 60) | ((uint64_t)tabela_raiz >> 12);     /* modo Sv39 */
    __asm__ volatile("csrw satp, %0\n sfence.vma" ::"r"(satp));
}

/* ---------- tarefas em modo-S ---------- */
static uint8_t pilha_a[8192] __attribute__((aligned(16)));
static uint8_t pilha_b[8192] __attribute__((aligned(16)));
static Tarefa tarefas[2];
static int atual = 0;
static int lancadas = 0;

static void tarefa_putc(char c) { *uart = (uint8_t)c; }
static void espera(void) { for (volatile int i = 0; i < 20000; i += 1) {} }

static void syscall_yield_e_teste(void) {
    register uint64_t a7 __asm__("a7") = 42;
    __asm__ volatile("ecall" : "+r"(a7));
}

static void tarefa_a(void) {
    int tocou_falta = 0;
    for (;;) {
        if (!tocou_falta) {
            syscall_yield_e_teste(); /* a7=42: SYSCALL_OK, volta aqui */
            volatile uint64_t *p = (volatile uint64_t *)VA_FALTA;
            uint64_t v = *p;         /* FAULT: o kernel mapeia e reexecuta */
            (void)v;
            tocou_falta = 1;
        }
        tarefa_putc('A');
        espera();
    }
}

static void tarefa_b(void) {
    for (;;) {
        tarefa_putc('B');
        espera();
    }
}

/* ---------- tratador de traps (modo-M, C) ---------- */
static uint64_t ticks = 0;
static int timer_ok = 0, syscall_ok = 0, falta_ok = 0, trocas = 0;
static TrapFrame saida;

static void desligar(uint64_t codigo) {
    *(volatile uint32_t *)TEST_DEVICE = (uint32_t)codigo;
    for (;;) { __asm__ volatile("wfi"); }
}

extern "C" TrapFrame *trap_handler(TrapFrame *tf) {
    uint64_t causa, tval;
    __asm__ volatile("csrr %0, mcause" : "=r"(causa));
    __asm__ volatile("csrr %0, mtval" : "=r"(tval));

    if ((int64_t)causa < 0) { /* interrupcao */
        if ((causa & 0xFF) == 7) { /* timer da maquina (CLINT) */
            uint64_t agora = *(volatile uint64_t *)CLINT_MTIME;
            *(volatile uint64_t *)CLINT_MTIMECMP = agora + INTERVALO_TICKS;
            ticks += 1;
            if (ticks >= 3 && !timer_ok) { timer_ok = 1; kputs("TIMER_OK\n"); }
            if (lancadas) {
                tarefas[atual].salvo = *tf;
                tarefas[atual].rodou = 1;
                atual = 1 - atual;
                trocas += 1;
                if (trocas >= 10 && timer_ok && syscall_ok && falta_ok && hart1_ok &&
                    tarefas[0].rodou && tarefas[1].rodou) {
                    kputs("SCHED_OK\nDONE\n");
                    desligar(0x5555);
                }
                saida = tarefas[atual].salvo;
                return &saida;
            }
        }
        return tf;
    }

    switch (causa) {
        case 9: /* ecall de modo-S: nossa "syscall" */
            if (tf->f[F_A7] == 42 && !syscall_ok) {
                syscall_ok = 1;
                kputs("SYSCALL_OK\n");
            }
            tf->f[F_MEPC] += 4;
            return tf;
        case 13: /* falta de pagina de leitura */
        case 15: /* falta de pagina de escrita */
            if (tval == VA_FALTA && !falta_ok) {
                mapear_giga(VA_FALTA, 0x80000000UL, PTE_R | PTE_W);
                __asm__ volatile("sfence.vma");
                falta_ok = 1;
                kputs("FAULT_OK\n");
                return tf; /* mepc intacto: a instrucao reexecuta */
            }
            kputs("FALTA_INESPERADA\n");
            desligar(0x3333);
            return tf;
        default:
            kputs("TRAP_INESPERADA causa=");
            kputhex(causa);
            kputs(" mtval=");
            kputhex(tval);
            kputs(" mepc=");
            kputhex(tf->f[F_MEPC]);
            kputc('\n');
            desligar(0x3333);
            return tf;
    }
}

/* ---------- kmain (hart 0, modo-M) ---------- */
extern "C" void lancar_tarefa(TrapFrame *tf);

static void preparar_tarefa(int i, void (*entrada)(void), uint8_t *pilha) {
    for (int k = 0; k < 32; k++) tarefas[i].salvo.f[k] = 0;
    tarefas[i].salvo.f[F_SP] = (uint64_t)(pilha + 8192);
    tarefas[i].salvo.f[F_MEPC] = (uint64_t)entrada;
    tarefas[i].rodou = 0;
}

extern "C" void kmain(void) {
    /* Sem firmware (OpenSBI), a PMP nasce fechada: modo-S nao acessaria
       nem a RAM. Abrir a regiao inteira (TOR ate o topo) antes de descer. */
    __asm__ volatile("csrw pmpaddr0, %0\n csrw pmpcfg0, %1" ::"r"(-1UL), "r"(0xFUL));
    montar_paginacao();

    /* libera o hart 1 e espera o anuncio dele antes de seguir */
    liberar_harts = 1;
    for (volatile int i = 0; i < 5000000 && !hart1_ok; i += 1) {}
    kputs("BOOT_OK\n");

    preparar_tarefa(0, tarefa_a, pilha_a);
    preparar_tarefa(1, tarefa_b, pilha_b);

    /* timer do CLINT e interrupcao de timer em modo-M */
    *(volatile uint64_t *)CLINT_MTIMECMP =
        *(volatile uint64_t *)CLINT_MTIME + INTERVALO_TICKS;
    __asm__ volatile("csrw mie, %0" ::"r"(1UL << 7));
    /* mret para modo-S: MPP=01, MPIE=1 */
    uint64_t mstatus;
    __asm__ volatile("csrr %0, mstatus" : "=r"(mstatus));
    mstatus &= ~(3UL << 11);
    mstatus |= (1UL << 11) | (1UL << 7);
    __asm__ volatile("csrw mstatus, %0" ::"r"(mstatus));

    atual = 0;
    lancadas = 1;
    lancar_tarefa(&tarefas[0].salvo);
    desligar(0x3333); /* nunca alcancado */
}
