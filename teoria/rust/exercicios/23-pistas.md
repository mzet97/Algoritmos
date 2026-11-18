---
layout: default
---

# Aula 23 (Rust): pistas graduais

Volte aos [exercicios](../23.md#exercícios-obrigatórios) antes de abrir as solucoes.

1. Multiplos binarios: `[[bin]]` nomeia cada alvo; `cargo run --bin server` escolhe qual sobe.
2. Streaming server-side: o retorno e um `Stream` de respostas, nao uma resposta; `tokio::time::sleep` entre itens da o ritmo.
3. O cliente consome o stream com `while let Some(item) = stream.next().await`.
