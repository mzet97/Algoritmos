---
layout: default
---

# Aula 04: pistas graduais

Volte aos [exercicios](../04.md#19-exercicios-obrigatorios) antes de abrir as solucoes.

1. Declare `int v[5]` e use `size_t n = sizeof v / sizeof v[0]` no mesmo escopo do array. O ultimo indice valido e `n - 1` quando `n > 0`.
2. Use `*(v + i)` apenas para `i < n`; nao desreferencie `v + n`.
3. Reserve quatro posicoes para tres letras e um `\0`. Verifique a capacidade antes de chamar `strlen`.
4. `strlen` continua lendo ate encontrar o primeiro byte zero; o tipo `char *` nao informa quantos bytes estao disponiveis.
5. Dentro de uma funcao com parametro `int v[]`, a declaracao e ajustada para ponteiro. Compare `sizeof v` no escopo do array com `sizeof parametro` dentro da funcao.
