---
layout: default
---

# Aula 01 (Rust): pistas graduais

Volte aos [exercicios](../01.md#15-exercícios-obrigatórios) antes de abrir as solucoes.

1. `std::mem::size_of::<T>()` responde o tamanho sem precisar de uma variavel de cada tipo.
2. Na conversao `200i32 as u8`: pense no que cabe em 8 bits e no que o `as` faz com o restante.
3. `mut` muda o valor da mesma ligacao; shadowing cria uma ligacao nova — qual dos dois o laco de soma pede?
4. Debug e release tratam overflow de forma diferente por padrao; os tres metodos nomeados existem para voce escolher sem depender do perfil.
5. No desenho da stack: tipos tem tamanhos diferentes e enderecos respeitam alinhamento — sobra espaco entre vizinhos.
