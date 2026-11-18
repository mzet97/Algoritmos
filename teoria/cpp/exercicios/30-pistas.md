---
layout: default
---

# Aula 30 (C++): pistas graduais

Volte aos [exercicios](../30.md#21-exercicios-obrigatorios) antes de abrir as solucoes. (As pontes executadas sao [30a](30a_pistas.md), modelo de GPU, e [30b](30b_pistas.md), SIMD.)

1. Compare os tres corpos: escalar puro, com desvio por elemento e com ponteiros `__restrict`.
2. O relatorio `-fopt-info-vec` e a evidencia — anote versao e flags junto.
3. `__restrict` e a sua promessa de nao-alias; sem ela, o compilador protege o caso sobreposto.
4. SoA alimenta o vetor com elementos consecutivos do mesmo campo; AoS mistura campos.
5. Intrinsicos so com medicao de que o autovetorizador nao basta neste kernel.
