---
layout: default
---

# Aula 01 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../01.md#15-exercícios-obrigatórios). Programa unico, compilado e executado nesta rodada:

```rust
use std::mem::size_of;

fn main() {
    // Exercicio 1: tipos e tamanhos
    println!("i8={} u8={} i32={} u32={} i64={} u64={} f32={} f64={} bool={} char={}",
        size_of::<i8>(), size_of::<u8>(), size_of::<i32>(), size_of::<u32>(),
        size_of::<i64>(), size_of::<u64>(), size_of::<f32>(), size_of::<f64>(),
        size_of::<bool>(), size_of::<char>());
    let caractere: char = 'á';
    println!("char exemplo: {} ({} bytes)", caractere, size_of::<char>());

    // Exercicio 2: conversoes explicitas
    let x: i32 = 200;
    println!("como u8: {}", x as u8);     // 200 cabe em u8 (0..=255)
    println!("como f64: {}", x as f64);   // 200
    println!("como i64: {}", x as i64);   // 200
    let y: i32 = 300;
    println!("300 como u8: {}", y as u8); // 44: 'as' trunca em tipos inteiros

    // Exercicio 3: mut acumula; shadowing cria ligacao nova por iteracao
    let mut soma_mut = 0;
    for i in 1..=10 {
        soma_mut += i;
    }
    let mut soma_shadow = 0;
    for i in 1..=10 {
        let soma_shadow_novo = soma_shadow + i; // sombra temporaria
        soma_shadow = soma_shadow_novo;
    }
    // Forma idiomatica de shadowing para o mesmo calculo:
    let soma = (1..=10).fold(0, |acc, i| acc + i);
    println!("somas: {} {} {}", soma_mut, soma_shadow, soma); // 55 55 55

    // Exercicio 4: overflow com u8 = 250 + 10
    let base: u8 = 250;
    println!("wrapping: {}", base.wrapping_add(10));   // 4
    println!("saturating: {}", base.saturating_add(10)); // 255
    println!("checked: {:?}", base.checked_add(10));   // None
    // Com '+': panic em debug; em release, wrap silencioso (sem checagem).
}
```

2. `200` cabe em `u8`, entao nada se perde neste caso; o comportamento do `as` entre inteiros e truncar para os bits que cabem (`300` vira `44`). Conversoes que podem perder devem ser explicitas no codigo justamente para essa decisao ficar visivel — `TryFrom` devolveria erro em vez de truncar.

3. Para acumular num laco, `mut` e o desenho direto: ha **um** acumulador cujo valor muda. Shadowing cria uma ligacao nova a cada passo, o que combina com transformacoes ("o novo valor derivado do anterior, possivelmente de outro tipo"), nao com um contador que vive mudando. A soma com `fold` mostra a terceira forma: o acumulador vive dentro do iterador.

4. Em debug, `+` com overflow faz panic (o programa para e diz onde). Em release padrao, ele envolve silenciosamente (`250 + 10` viraria `4` sem aviso). `wrapping_add` declara "envolver e o que eu quero"; `saturating_add` para no limite (255); `checked_add` devolve `Option` e deixa a decisao com voce. Escolher o metodo e escrever a politica no tipo, nao depender do perfil de compilacao.

5. Modelo mental da stack (tamanhos nesta plataforma): `a: i8` ocupa 1 byte; `b: f32`, 4 bytes com alinhamento 4 (pode haver 3 bytes de preenchimento depois de `a`); `c: bool`, 1 byte; `d: char`, 4 bytes alinhados a 4; `e: u64`, 8 bytes alinhados a 8. O desenho correto mostra buracos de alinhamento entre vizinhos — a ordem exata das variaveis na stack e escolha do compilador, os tamanhos e alinhamentos nao.

**Autoverificacao:** tamanhos `1 1 4 4 8 8 4 8 1 4`, `300 as u8` explicado sem rodar e os tres comportamentos de overflow nomeados com seus resultados (4, 255, `None`).
