---
layout: default
---

# Aula 22 (Rust): pistas graduais

Volte aos [exercicios](../22.md#exercícios-obrigatórios) antes de abrir as solucoes.

1. Transacao: os dois UPDATEs usam `&mut *tx`; se qualquer um falhar, nada de `commit` — o rollback e o destino padrao ao descartar.
2. Saldo insuficiente precisa ser erro **antes** do primeiro UPDATE ou validado dentro da transacao.
3. `sqlx::migrate!` embute as migrations no binario em tempo de compilacao; o diretorio `./migrations` e relativo a raiz do crate.
