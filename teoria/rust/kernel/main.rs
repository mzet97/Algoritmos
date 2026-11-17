//! Kernel didatico RISC-V em Rust no_std (QEMU virt, modo-M dono das
//! traps): o mesmo percurso das trilhas de C/C++ — boot, PMP, Sv39,
//! descida para modo-S, syscall por ecall, falta sob demanda e
//! escalonador preemptivo por timer CLINT entre duas tarefas.
//! Marcadores: BOOT_OK HART1_OK TIMER_OK SYSCALL_OK FAULT_OK SCHED_OK DONE.
#![no_std]
#![no_main]

use core::arch::asm;
use core::panic::PanicInfo;
use core::ptr::{read_volatile, write_volatile};

const UART: usize = 0x1000_0000;
const CLINT_MTIMECMP: usize = 0x200_4000;
const CLINT_MTIME: usize = 0x200_BFF8;
const TEST_DEVICE: usize = 0x100_000;
const INTERVALO: u64 = 100_000; // MTIME 10 MHz -> 10 ms
const VA_FALTA: usize = 0x1_0000_0000; // 4 GiB, sem mapa no boot

unsafe extern "C" {
    fn lancar_tarefa(tf: *mut TrapFrame) -> !;
}

#[repr(C)]
#[derive(Clone, Copy)]
struct TrapFrame {
    f: [u64; 32], // f[0]=x1, f[1]=sp, f[2]=x3 .. f[30]=x31, f[31]=mepc
}

const FRAME_VAZIO: TrapFrame = TrapFrame { f: [0; 32] };

#[derive(Clone, Copy)]
struct Tarefa {
    salvo: TrapFrame,
    rodou: bool,
}

static mut TAREFAS: [Tarefa; 2] = [
    Tarefa { salvo: FRAME_VAZIO, rodou: false },
    Tarefa { salvo: FRAME_VAZIO, rodou: false },
];
static mut ATUAL: usize = 0;
static mut LANCADAS: bool = false;
static mut TICKS: u64 = 0;
static mut TIMER_OK: bool = false;
static mut SYSCALL_OK: bool = false;
static mut FALTA_OK: bool = false;
static mut TROCAS: u64 = 0;
static mut SAIDA: TrapFrame = FRAME_VAZIO;
static mut PILHA_A: [u8; 8192] = [0; 8192];
static mut PILHA_B: [u8; 8192] = [0; 8192];

#[unsafe(no_mangle)]
pub static mut liberar_harts: u32 = 0;
#[unsafe(no_mangle)]
pub static mut hart1_ok: u32 = 0;

fn kputc(c: u8) {
    unsafe { write_volatile(UART as *mut u8, c) }
}
fn kputs(s: &str) {
    for b in s.bytes() {
        kputc(b);
    }
}
fn kputhex(v: u64) {
    kputs("0x");
    let mut i = 60;
    loop {
        let d = ((v >> i) & 0xF) as u8;
        kputc(if d < 10 { b'0' + d } else { b'a' + d - 10 });
        if i == 0 {
            break;
        }
        i -= 4;
    }
}

fn desligar(codigo: u32) -> ! {
    unsafe {
        write_volatile(TEST_DEVICE as *mut u32, codigo);
        loop {
            asm!("wfi");
        }
    }
}

// ---------- Sv39 com gigapages ----------
#[repr(C, align(4096))]
struct TabelaRaiz([u64; 512]);
static mut TABELA_RAIZ: TabelaRaiz = TabelaRaiz([0; 512]);
const PTE_V: u64 = 1 << 0;
const PTE_R: u64 = 1 << 1;
const PTE_W: u64 = 1 << 2;
const PTE_X: u64 = 1 << 3;
const PTE_AD: u64 = (1 << 6) | (1 << 7);

unsafe fn mapear_giga(va: usize, pa: usize, flags: u64) {
    let raiz = core::ptr::addr_of_mut!(TABELA_RAIZ.0) as *mut u64;
    unsafe {
        *raiz.add((va >> 30) & 0x1FF) = (((pa >> 12) as u64) << 10) | flags | PTE_V | PTE_AD;
    }
}

unsafe fn montar_paginacao() {
    unsafe {
        mapear_giga(0, 0, PTE_R | PTE_W);
        mapear_giga(0x8000_0000, 0x8000_0000, PTE_R | PTE_W | PTE_X);
    }
    let satp = (8u64 << 60) | ((core::ptr::addr_of!(TABELA_RAIZ) as u64) >> 12);
    debug_assert_eq!(core::ptr::addr_of!(TABELA_RAIZ) as u64 % 4096, 0);
    unsafe {
        asm!("csrw satp, {0}\n sfence.vma", in(reg) satp);
    }
}

// ---------- tarefas em modo-S ----------
fn espera() {
    for _ in 0..20000 {
        unsafe { core::ptr::read_volatile(&0u8) };
    }
}

fn tarefa_a() -> ! {
    let mut tocou = false;
    loop {
        if !tocou {
            let mut a7: u64 = 42;
            unsafe {
                asm!("ecall", inout("a7") a7);
                let v = read_volatile(VA_FALTA as *const u64);
                core::hint::black_box(v);
            }
            tocou = true;
        }
        kputc(b'A');
        espera();
    }
}

fn tarefa_b() -> ! {
    loop {
        kputc(b'B');
        espera();
    }
}

// ---------- tratador de traps ----------
#[unsafe(no_mangle)]
extern "C" fn trap_handler(tf: *mut TrapFrame) -> *mut TrapFrame {
    unsafe {
        let mut causa: u64;
        let mut tval: u64;
        asm!("csrr {0}, mcause", out(reg) causa);
        asm!("csrr {0}, mtval", out(reg) tval);
        if (causa as i64) < 0 {
            if (causa & 0xFF) == 7 {
                let agora = read_volatile(CLINT_MTIME as *const u64);
                write_volatile(CLINT_MTIMECMP as *mut u64, agora + INTERVALO);
                TICKS += 1;
                if TICKS >= 3 && !TIMER_OK {
                    TIMER_OK = true;
                    kputs("TIMER_OK\n");
                }
                if LANCADAS {
                    let atual = ATUAL;
                    TAREFAS[atual].salvo = *tf;
                    TAREFAS[atual].rodou = true;
                    ATUAL = 1 - atual;
                    TROCAS += 1;
                    if TROCAS >= 10 && TIMER_OK && SYSCALL_OK && FALTA_OK
                        && hart1_ok != 0 && TAREFAS[0].rodou && TAREFAS[1].rodou
                    {
                        kputs("SCHED_OK\nDONE\n");
                        desligar(0x5555);
                    }
                    SAIDA = TAREFAS[ATUAL].salvo;
                    return core::ptr::addr_of_mut!(SAIDA);
                }
            }
            return tf;
        }
        match causa {
            9 => {
                if (*tf).f[16] == 42 && !SYSCALL_OK {
                    SYSCALL_OK = true;
                    kputs("SYSCALL_OK\n");
                }
                (*tf).f[31] += 4;
                tf
            }
            13 | 15 => {
                if tval as usize == VA_FALTA && !FALTA_OK {
                    mapear_giga(VA_FALTA, 0x8000_0000, PTE_R | PTE_W);
                    asm!("sfence.vma");
                    FALTA_OK = true;
                    kputs("FAULT_OK\n");
                    return tf; // mepc intacto: reexecuta
                }
                kputs("FALTA_INESPERADA\n");
                desligar(0x3333);
            }
            _ => {
                kputs("TRAP_INESPERADA causa=");
                kputhex(causa);
                kputs(" mtval=");
                kputhex(tval);
                kputc(b'\n');
                desligar(0x3333);
            }
        }
    }
}

fn preparar(i: usize, entrada: fn() -> !, pilha: *const u8) {
    unsafe {
        TAREFAS[i].salvo = FRAME_VAZIO;
        TAREFAS[i].salvo.f[1] = (pilha as usize + 8192) as u64;
        TAREFAS[i].salvo.f[31] = entrada as usize as u64;
        TAREFAS[i].rodou = false;
    }
}

#[unsafe(no_mangle)]
extern "C" fn kmain() -> ! {
    unsafe {
        // PMP aberta antes de descer (sem firmware, nasce fechada).
        asm!("csrw pmpaddr0, {0}\n csrw pmpcfg0, {1}", in(reg) u64::MAX, in(reg) 0xFu64);
        montar_paginacao();
        liberar_harts = 1;
        let mut espera_hart = 0;
        while hart1_ok == 0 && espera_hart < 5_000_000 {
            core::hint::spin_loop();
            espera_hart += 1;
        }
        kputs("BOOT_OK\n");
        preparar(0, tarefa_a, core::ptr::addr_of!(PILHA_A) as *const u8);
        preparar(1, tarefa_b, core::ptr::addr_of!(PILHA_B) as *const u8);
        write_volatile(
            CLINT_MTIMECMP as *mut u64,
            read_volatile(CLINT_MTIME as *const u64) + INTERVALO,
        );
        asm!("csrw mie, {0}", in(reg) 1u64 << 7);
        let mut mstatus: u64;
        asm!("csrr {0}, mstatus", out(reg) mstatus);
        mstatus &= !(3u64 << 11);
        mstatus |= (1u64 << 11) | (1u64 << 7);
        asm!("csrw mstatus, {0}", in(reg) mstatus);
        ATUAL = 0;
        LANCADAS = true;
        let tf = core::ptr::addr_of_mut!(TAREFAS) as *mut TrapFrame;
        lancar_tarefa(tf);
    }
}

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    kputs("PANIC\n");
    desligar(0x3333);
}
