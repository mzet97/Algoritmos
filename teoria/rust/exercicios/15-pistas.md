---
layout: default
---

# Aula 15 (Rust): pistas graduais

Volte aos [exercicios](../15.md#15-exercícios-obrigatórios) antes de abrir as solucoes.

1. `meu_split_at_mut`: dois `from_raw_parts_mut` a partir do mesmo ponteiro, com o assert antes do unsafe.
2. `MeuVec` segura ponteiro, comprimento e capacidade; `Drop` precisa destruir elementos e a alocacao.
3. Bits de `u32`: `transmute` confia em voce; `from_ne_bytes` declara a ordem e dispensa unsafe.
4. Lista com `*mut Node`: cada `Box::into_raw` pede um `Box::from_raw` correspondente — conte.
5. Union le um campo de cada vez; compare sempre com `to_bits`, que e a forma segura de hoje.
6. `Buffer` com `alloc`: capacidade em **elementos** no seu contrato, bytes no `Layout`.
7. Nos tres metodos da std: o unsafe fica dentro, e a API segura cobra as invariantes na entrada.
