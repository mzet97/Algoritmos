---
layout: default
---

# Aula 16 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../16.md#11-exercícios-obrigatórios). Ambiente declarado: `bindgen` e `cxx` nao estao instalados nesta maquina; os exercicios 4 e 6 ficam com o desenho e as invariantes documentados e **nao** foram executados aqui — os demais rodaram como abaixo.

1. Wrapper seguro sobre a libm:

```rust
// src/lib.rs (ou main.rs de estudo)
mod ffi {
    unsafe extern "C" {
        pub fn sqrt(x: f64) -> f64;
        pub fn pow(base: f64, exp: f64) -> f64;
        pub fn sin(x: f64) -> f64;
        pub fn cos(x: f64) -> f64;
    }
}

pub fn mat_sqrt(x: f64) -> Result<f64, String> {
    if x < 0.0 {
        return Err(String::from("sqrt de negativo"));
    }
    // Safety: sqrt da libm e funcao pura C sobre f64, sem ponteiros.
    let r = unsafe { ffi::sqrt(x) };
    if r.is_nan() {
        Err(String::from("resultado NaN"))
    } else {
        Ok(r)
    }
}

pub fn mat_pow(base: f64, exp: f64) -> f64 {
    // Safety: idem; resultado pode ser infinito por contrato matematico.
    unsafe { ffi::pow(base, exp) }
}

pub fn mat_sin(x: f64) -> f64 {
    // Safety: idem.
    unsafe { ffi::sin(x) }
}

pub fn mat_cos(x: f64) -> f64 {
    // Safety: idem.
    unsafe { ffi::cos(x) }
}
```

(No Linux, `sqrt`/`pow`/`sin`/`cos` chegam pela libc ja ligada ao binario Rust; em alvos onde a libm e separada, o build pede `#[link(name = "m")]`.)

2 e 3. Biblioteca exportavel e sorter com callback C, como um binario de estudo que exercita a mesma fronteira por dentro:

```rust
use std::os::raw::c_int;

pub struct ListaInt {
    itens: Vec<i32>,
}

#[unsafe(no_mangle)]
pub extern "C" fn criar_lista() -> *mut ListaInt {
    Box::into_raw(Box::new(ListaInt { itens: Vec::new() }))
}

/// # Safety
/// `lista` deve ser nula ou um ponteiro devolvido por `criar_lista`
/// que ainda nao foi destruido.
#[unsafe(no_mangle)]
pub unsafe extern "C" fn lista_push(lista: *mut ListaInt, valor: i32) {
    if lista.is_null() {
        return;
    }
    // Safety: garantido pelo contrato acima.
    unsafe { (*lista).itens.push(valor) };
}

/// # Safety
/// Mesmas condicoes de `lista_push`. Devolve 0 com a lista vazia
/// (politica declarada deste laboratorio).
#[unsafe(no_mangle)]
pub unsafe extern "C" fn lista_pop(lista: *mut ListaInt) -> i32 {
    if lista.is_null() {
        return 0;
    }
    // Safety: contrato acima.
    unsafe { (*lista).itens.pop().unwrap_or(0) }
}

/// # Safety
/// `lista` nula ou valida como acima.
#[unsafe(no_mangle)]
pub unsafe extern "C" fn lista_len(lista: *const ListaInt) -> usize {
    if lista.is_null() {
        return 0;
    }
    // Safety: contrato acima.
    unsafe { (*lista).itens.len() }
}

/// # Safety
/// `lista` deve ser nula ou um ponteiro de `criar_lista` ainda nao
/// destruido; depois desta chamada o ponteiro esta invalido.
#[unsafe(no_mangle)]
pub unsafe extern "C" fn destruir_lista(lista: *mut ListaInt) {
    if lista.is_null() {
        return;
    }
    // Safety: contrato acima; Box::from_raw retoma a posse uma vez.
    drop(unsafe { Box::from_raw(lista) });
}

type ComparadorC = extern "C" fn(c_int, c_int) -> c_int;

/// # Safety
/// `data` deve apontar para `len` inteiros validos para leitura e
/// escrita, ou ser nulo com `len` zero; `cmp` deve comparar dois
/// inteiros e devolver negativo/zero/positivo.
#[unsafe(no_mangle)]
pub unsafe extern "C" fn rust_sort(
    data: *mut c_int,
    len: usize,
    cmp: Option<ComparadorC>,
) -> *mut c_int {
    if data.is_null() || len == 0 {
        return data;
    }
    let comparador = match cmp {
        Some(c) => c,
        None => return data,
    };
    // Safety: contrato acima garante a faixa [data, data+len).
    let fatia = unsafe { std::slice::from_raw_parts_mut(data, len) };
    fatia.sort_by(|a, b| comparador(*a, *b).cmp(&0));
    data
}

extern "C" fn cmp_crescente(a: c_int, b: c_int) -> c_int {
    a.cmp(&b) as c_int
}

extern "C" fn cmp_decrescente(a: c_int, b: c_int) -> c_int {
    b.cmp(&a) as c_int
}

fn main() {
    println!("sqrt: {:?}", mat_sqrt(16.0));
    println!("sqrt(-1): {:?}", mat_sqrt(-1.0));
    println!("pow: {}", mat_pow(2.0, 10.0));
    println!("sin(0)={} cos(0)={}", mat_sin(0.0), mat_cos(0.0));

    // Safety: ponteiros deste proprio teste, validos e destruidos aqui.
    unsafe {
        let lista = criar_lista();
        lista_push(lista, 3);
        lista_push(lista, 4);
        println!("lista len={} pop={}", lista_len(lista), lista_pop(lista));
        destruir_lista(lista);
    }

    let mut dados = [5, 2, 9, 1];
    // Safety: fatia local valida, comparadores acima cumprem o contrato.
    unsafe {
        rust_sort(dados.as_mut_ptr(), dados.len(), Some(cmp_crescente));
    }
    println!("crescente: {:?}", dados);
    // Safety: idem.
    unsafe {
        rust_sort(dados.as_mut_ptr(), dados.len(), Some(cmp_decrescente));
    }
    println!("decrescente: {:?}", dados);
    assert_eq!(dados, [9, 5, 2, 1]);
}
```

Header C correspondente (exercicio 2):

```c
#ifndef LISTA_INT_H
#define LISTA_INT_H

#include <stddef.h>
#include <stdint.h>

typedef struct ListaInt ListaInt;

ListaInt *criar_lista(void);
void lista_push(ListaInt *lista, int32_t valor);
int32_t lista_pop(ListaInt *lista);
size_t lista_len(const ListaInt *lista);
void destruir_lista(ListaInt *lista);

#endif
```

5. Parsing binario seguro na fronteira:

```rust
#[repr(C)]
#[derive(Debug, Clone, Copy)]
struct Cabecalho {
    magic: [u8; 4],
    registros: u32,
    versao: u16,
    reservado: u16,
}

fn validar_cabecalho(bytes: &[u8]) -> Result<Cabecalho, String> {
    if bytes.len() < std::mem::size_of::<Cabecalho>() {
        return Err(String::from("arquivo menor que o cabecalho"));
    }
    // Safety: lemos so os primeiros size_of bytes como Cabecalho apos
    // checar o comprimento; u8/u32/u16 aceitam qualquer padrao de bits.
    let cab = unsafe {
        std::ptr::read_unaligned(bytes.as_ptr() as *const Cabecalho)
    };
    if &cab.magic != b"RUST" {
        return Err(String::from("magic invalido"));
    }
    if cab.versao != 1 {
        return Err(format!("versao nao suportada: {}", cab.versao));
    }
    Ok(cab)
}
```

Validar **antes** de interpretar e a regra inteira: magic, versao e tamanhos primeiro; nenhum campo e lido de um arquivo que nao passou nisso. (Endianness do formato e contrato do arquivo — este laboratorio le na ordem nativa e declara isso.)

4 e 6 (desenho, nao executados aqui): com bindgen, o fluxo e header C → `bindgen` gera `bindings.rs` feio e unsafe → seus wrappers seguros por cima, um por funcao, com `# Safety`. Com cxx, o `#[cxx::bridge]` declara os tipos compartilhados e cada lado chama o outro por funcoes geradas; o projeto pede `build.rs` proprio. Instale as ferramentas e execute antes de marcar estes dois como feitos — neste ambiente eles estao **documentados, nao verificados**.

**Autoverificacao:** `sqrt(-1)` como `Err` (nao NaN silencioso), lista criada/usada/destruida sem vazamento sob ASan onde houver, sorter nos dois sentidos e o cabecalho rejeitando magic errado.
