---
layout: default
---

# Aula 18 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../18.md#exercícios-obrigatórios).

1. Refatoracao zero-cost — versao inicial alocando contra versao com iterador:

```rust
// Versao inicial (aloca): Vec<String> -> Vec<String>
fn comeca_com_a_alocando(nomes: Vec<String>) -> Vec<String> {
    nomes
        .into_iter()
        .filter(|n| n.starts_with('A'))
        .collect()
}

// Versao zero-cost: aceita fatias e devolve um iterador preguiçoso.
// Nada aloca: sem Vec novo, sem String nova, sem copia dos nomes.
fn comeca_com_a<'a>(nomes: &'a [&'a str]) -> impl Iterator<Item = &'a str> + 'a {
    nomes.iter().copied().filter(|n| n.starts_with('A'))
}

fn main() {
    let nomes = vec![
        String::from("Ana"),
        String::from("Bruno"),
        String::from("Alice"),
    ];
    let alocando = comeca_com_a_alocando(nomes);
    println!("alocando: {:?}", alocando);

    let fatias: Vec<&str> = vec!["Ana", "Bruno", "Alice", "andre"];
    let selecionados: Vec<&str> = comeca_com_a(&fatias).collect();
    println!("zero-cost: {:?}", selecionados);
    assert_eq!(selecionados, vec!["Ana", "Alice"]);
    // 'andre' fica de fora: starts_with('A') distingue maiuscula.
}
```

A diferenca nao e so performance: a segunda versao aceita qualquer fonte de `&str` (literais, fatias de um buffer, campos de structs) e so paga pelo que o consumidor realmente puxar do iterador. "Zero-cost" aqui significa: a abstracao compila para o mesmo laco que voce escreveria a mao.

2. Troca de alocador (projeto a parte, executado nesta rodada num scratch com a crate publica):

```toml
# Cargo.toml
[dependencies]
mimalloc = "0.1"
```

```rust
// src/main.rs
#[global_allocator]
static GLOBAL: mimalloc::MiMalloc = mimalloc::MiMalloc;

fn main() {
    let mut total = 0usize;
    for i in 0..1_000_000 {
        let v: Vec<usize> = vec![i; 4];
        total += v.len();
    }
    println!("total: {}", total);
}
```

Rode o mesmo `main` sem as duas linhas do alocador e compare medianas de 5 execucoes nesta maquina antes de concluir qualquer coisa: nesta rodada, neste microbenchmark de vetores pequenos e curtos, o alocador padrao foi **mais rapido** que o mimalloc — alocador alternativo paga em padroes de alocacao especificos (muitas threads, vidas longas, fragmentacao), nao por decreto.

3. Perfil de release agressivo, no `Cargo.toml` do workspace:

```toml
[profile.max_perf]
inherits = "release"
lto = true
codegen-units = 1
opt-level = 3
```

Use com `cargo build --profile max_perf`. `lto` e `codegen-units = 1` trocam tempo de compilacao por otimizacao entre modulos; o ganho se mede no seu benchmark (aula 17), nunca se assume.

**Autoverificacao:** a versao zero-cost sem nenhum `collect` interno, `andre` fora da lista (caixa importa) e o benchmark de alocador com medianas dos dois lados.
