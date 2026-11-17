---
layout: default
---

# Aula 26: pistas graduais

Volte aos [exercicios](../26.md#21-exercicios-obrigatorios) antes de abrir as solucoes.

1. `sb_append_char` e o caso particular de anexar um byte: garanta capacidade para um, escreva, avance o tamanho.
2. Bytes binarios pedem comprimento explicito em todas as operacoes; o `\0` vira so mais um byte.
3. No crescimento 1.5 contra 2, reuse a medicao da [aula 24](24-solucoes.md): realocacoes e capacidade ociosa.
4. `strlen` em loop recalcula o que o builder ja sabe; onde o comprimento deveria morar?
5. Ponteiro interno guardado aponta para um buffer que `realloc` pode mover; desenhe o antes/depois.
