// Aula 16c Rust — ISA, ABI e layout: o que o hardware promete.
//
// Tres promessas verificadas em um processo: (1) o layout que o repr
// garante (`offset_of!` + tamanhos, deterministico); (2) a ordem de
// bytes nativa (roundtrip `to_ne_bytes`, deterministico); (3) a
// fronteira extern "C" chamada pelo nome e por ponteiro (a mesma ABI
// que o assembly do kernel usa). Nada aqui depende de crate externa.
//
// Compilar: rustc --edition 2024 -O -D warnings teoria/rust/exemplos/16c_isa_abi.rs -o /tmp/16c
// Executar: /tmp/16c
use std::mem::{align_of, offset_of, size_of};

#[repr(C)]
struct CabecalhoC {
    etiqueta: u8,
    comprimento: u32,
    total: u64,
}

// extern "C" com nome exportado: a ABI que o trap.S do kernel chama.
#[unsafe(no_mangle)]
pub extern "C" fn soma_c(a: u32, b: u32) -> u32 {
    a.wrapping_add(b)
}

fn arquitetura() -> &'static str {
    std::env::consts::ARCH
}

fn main() {
    println!("arch={}", arquitetura());
    assert!(matches!(
        arquitetura(),
        "x86_64" | "aarch64" | "riscv64" | "x86" | "aarch32"
    ));

    // 1. Layout: repr(C) tem ordem e deslocamentos contratuais.
    assert_eq!(offset_of!(CabecalhoC, etiqueta), 0);
    assert_eq!(offset_of!(CabecalhoC, comprimento), 4);
    assert_eq!(offset_of!(CabecalhoC, total), 8);
    assert_eq!(size_of::<CabecalhoC>(), 16);
    assert_eq!(align_of::<CabecalhoC>(), 8);
    println!(
        "layout repr(C): tam={} alin={} offs=(0,4,8)",
        size_of::<CabecalhoC>(),
        align_of::<CabecalhoC>()
    );
    // Leitura desalinhada sem UB: a API segura existe para isso.
    let bytes = [1u8, 2, 3, 4, 5, 6, 7, 8, 9];
    let v = unsafe { (bytes.as_ptr() as *const u32).read_unaligned() };
    assert_eq!(v, u32::from_ne_bytes([1, 2, 3, 4]));
    println!("read_unaligned ok: 0x{v:08x}");

    // 2. Ordem de bytes: o valor atravessa ida e volta intacto.
    let original: u32 = 0x01020304;
    assert_eq!(u32::from_ne_bytes(original.to_ne_bytes()), original);
    #[cfg(target_endian = "little")]
    assert_eq!(original.to_le_bytes(), [0x04, 0x03, 0x02, 0x01]);
    println!("endianness nativa ok: 0x{original:08x}");

    // 3. Fronteira C: pelo nome e por ponteiro de funcao.
    assert_eq!(soma_c(40, 2), 42);
    let f: extern "C" fn(u32, u32) -> u32 = soma_c;
    assert_eq!(f(40, 2), 42);
    println!("extern C ok: soma_c(40,2)=42 por nome e por ponteiro");

    println!("16c ok: ISA nomeada, layout contratado, ABI atravessada");
}
