/* Nucleo da aula 35: paginacao Sv39 com gigapages e falta sob demanda.
 * kmain liga o Sv39 (identidade para MMIO baixo e RAM), confirma o modo
 * lendo satp de volta e toca um endereco propositalmente sem mapa
 * (VA_FALTA, 4 GiB): a falta de pagina (causas 13/15) mapeia e a instrucao
 * reexecuta com mepc intacto. Qualquer falta fora desse endereco e defeito
 * do experimento e desliga com 0x3333.
 * Marcadores: BOOT_OK PAGING_ON_OK DEMAND_OK DONE. */
#include <stdint.h>

#define UART 0x10000000UL
#define TEST_DEVICE 0x100000UL

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

static int demanda_ok = 0;

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
        case 13: /* falta de pagina de leitura */
        case 15: /* falta de pagina de escrita */
            if (tval == VA_FALTA && !demanda_ok) {
                /* Gigapage exige fisico alinhado a 1 GiB: alias da RAM. */
                mapear_giga(VA_FALTA, 0x80000000UL, PTE_R | PTE_W);
                __asm__ volatile("sfence.vma");
                demanda_ok = 1;
                kputs("DEMAND_OK\n");
                return tf; /* mepc intacto: a instrucao reexecuta */
            }
            kputs("FALTA_INESPERADA mtval=");
            kputhex(tval);
            kputc('\n');
            desligar(0x3333);
            return tf;
        default:
            kputs("TRAP_INESPERADA causa=");
            kputhex(causa);
            kputc('\n');
            desligar(0x3333);
            return tf;
    }
}

/* Roda em modo-S: aqui o satp traduz de verdade. O toque em VA_FALTA
   gera falta de pagina (13/15), o tratador em modo-M mapeia e a escrita
   reexecuta com mepc intacto. */
static void modo_s_teste(void) {
    volatile uint64_t *p = (volatile uint64_t *)VA_FALTA;
    *p = 0xCAFEUL;           /* falta de escrita: mapeia sob demanda */
    if (*p != 0xCAFEUL) {    /* releitura apos o remapeamento */
        kputs("RELEITURA_DIVERGIU\n");
        desligar(0x3333);
    }
    kputs("DONE\n");
    desligar(0x5555);
}

extern "C" void kmain(void) {
    kputs("BOOT_OK\n");
    /* Sem firmware, a PMP nasce fechada para S/U: abrir antes de descer. */
    __asm__ volatile("csrw pmpaddr0, %0\n csrw pmpcfg0, %1" ::"r"(-1UL), "r"(0xFUL));
    mapear_giga(0x00000000UL, 0x00000000UL, PTE_R | PTE_W);
    mapear_giga(0x80000000UL, 0x80000000UL, PTE_R | PTE_W | PTE_X);
    uint64_t satp = (8UL << 60) | ((uint64_t)tabela_raiz >> 12);
    __asm__ volatile("csrw satp, %0\n sfence.vma" ::"r"(satp));
    uint64_t lido;
    __asm__ volatile("csrr %0, satp" : "=r"(lido));
    if ((lido >> 60) != 8) {
        kputs("SATP_NAO_LIGOU\n");
        desligar(0x3333);
    }
    kputs("PAGING_ON_OK\n");

    /* Desce a modo-S (MPP=01): so la o MMU traduz e a falta e testavel.
       O protocolo de troca de privilegio e tema da aula 36; aqui ele e
       apenas o pre-requisito fisico do teste de paginacao. */
    uint64_t mstatus;
    __asm__ volatile("csrr %0, mstatus" : "=r"(mstatus));
    mstatus &= ~(3UL << 11);
    mstatus |= (1UL << 11);
    __asm__ volatile("csrw mstatus, %0" ::"r"(mstatus));
    __asm__ volatile("csrw mepc, %0" ::"r"((uint64_t)modo_s_teste));
    __asm__ volatile("mret");
    desligar(0x3333); /* nunca alcancado */
}
