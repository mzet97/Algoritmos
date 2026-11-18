---
layout: default
---

# Aula 24 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../24.md#exercícios-obrigatórios). Ambiente: `std::simd` continua nightly — executado aqui com a toolchain **nightly instalada** (`cargo +nightly run --release` ou `rustup override set nightly` no projeto). Em stable, este codigo nem compila, e essa fronteira precisa estar escrita onde o aluno for rodar.

```rust
#![feature(portable_simd)]

use std::simd::prelude::*;
use std::simd::f32x8;
use std::time::Instant;

fn multiplica_escalar(a: &[f32], b: &[f32], c: &mut [f32]) {
    for i in 0..a.len() {
        c[i] = a[i] * b[i];
    }
}

fn multiplica_simd(a: &[f32], b: &[f32], c: &mut [f32]) {
    let blocos = a.len() / 8;
    for bloco in 0..blocos {
        let inicio = bloco * 8;
        let va = f32x8::from_slice(&a[inicio..]);
        let vb = f32x8::from_slice(&b[inicio..]);
        (va * vb).copy_to_slice(&mut c[inicio..]);
    }
    for i in blocos * 8..a.len() {
        c[i] = a[i] * b[i];
    }
}

fn maximo_escalar(v: &[f32]) -> f32 {
    v.iter().copied().fold(f32::NEG_INFINITY, f32::max)
}

fn maximo_simd(v: &[f32]) -> f32 {
    let mut acumulado = f32x8::splat(f32::NEG_INFINITY);
    let blocos = v.len() / 8;
    for bloco in 0..blocos {
        let pacote = f32x8::from_slice(&v[bloco * 8..]);
        acumulado = acumulado.simd_max(pacote);
    }
    let mut maior = acumulado.reduce_max();
    for i in blocos * 8..v.len() {
        maior = maior.max(v[i]);
    }
    maior
}

fn main() {
    let n = 1_000_003; // proposital: nao multiplo de 8
    let a: Vec<f32> = (0..n).map(|i| (i % 13) as f32 * 0.5).collect();
    let b: Vec<f32> = (0..n).map(|i| (i % 7) as f32 + 1.0).collect();
    let mut c1 = vec![0.0f32; n];
    let mut c2 = vec![0.0f32; n];

    let inicio = Instant::now();
    multiplica_escalar(&a, &b, &mut c1);
    println!("escalar: {:?}", inicio.elapsed());
    let inicio = Instant::now();
    multiplica_simd(&a, &b, &mut c2);
    println!("simd: {:?}", inicio.elapsed());
    assert_eq!(c1, c2);

    println!("maximo: escalar={} simd={}", maximo_escalar(&a), maximo_simd(&a));
    assert_eq!(maximo_escalar(&a), maximo_simd(&a));
    println!("resultados iguais, sobra incluida");
}
```

1 e 2 compartilham o mesmo esqueleto: blocos cheios de 8 pelo caminho largo e a **sobra escalar** no fim (1.000.003 nao e multiplo de 8 — esquecer a sobra e o bug classico que os testes com tamanho redondo nao pegam). No maximo, o `if` escalar por elemento vira `simd_max` entre pacotes: o acumulador guarda 8 maximos parciais e so reduz uma vez.

3. Assembly: o Godbolt nao foi acessado neste ambiente. O equivalente local executavel e compilar com `-C target-cpu=native` e inspecionar o assembly emitido (`cargo rustc --release -- --emit asm` ou `objdump -d`), procurando instrucoes de vetor compativeis com a sua CPU — o nome exato (`vaddps`, `vmulps`, NEON) depende dela. Declarar "vi no Godbolt" sem ter visto seria fabricar evidencia; faca um dos dois e anote qual.

**Autoverificacao:** vetores identicos com tamanho nao multiplo de 8, maximo igual nos dois caminhos e o relatorio/assembly da sua maquina anexado com a toolchain nomeada (nightly).
