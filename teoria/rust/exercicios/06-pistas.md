---
layout: default
---

# Aula 06 (Rust): pistas graduais

Volte aos [exercicios](../06.md#16-exercícios-obrigatórios) antes de abrir as solucoes.

1. Cada diretorio com codigo e um modulo: `mod.rs` declara os submodulos e re-exporta o que e publico.
2. Campo privado so muda por metodo; o erro ao acessar `saldo` e a visibilidade trabalhando.
3. `pub use circulo::area as area_circulo;` encurta o caminho para quem usa o modulo.
4. Teste mora junto do modulo testado (`#[cfg(test)]`) e roda com `cargo test` sem o binario.
