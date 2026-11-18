---
layout: default
---

# Aula 18 (Rust): pistas graduais

Volte aos [exercicios](../18.md#exercícios-obrigatórios) antes de abrir as solucoes.

1. Zero-cost: aceite `&[&str]` e devolva um iterador preguiçoso — nada de `Vec` novo nem `String` nova.
2. Trocar o alocador e uma linha de configuracao global; o benchmark precisa alocar muito para o efeito aparecer.
3. O perfil `[profile.max_perf]` mora no `Cargo.toml` do workspace, nao no codigo.
