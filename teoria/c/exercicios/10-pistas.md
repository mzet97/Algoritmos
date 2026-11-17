---
layout: default
---

# Aula 10: pistas graduais

Volte aos [exercicios](../10.md#16-exercicios-obrigatorios) antes de abrir as solucoes.

1. Reuse o experimento da aula 05: mesmos campos, ordens diferentes, `%zu` para imprimir.
2. `memcpy` para um `unsigned char[]` do mesmo tamanho do objeto e um laco imprimindo em hexadecimal; comece por um `int` com valor conhecido.
3. Para a lista de UB: recupere das aulas anteriores leitura fora do array, uso apos liberacao, divisao por zero de inteiros, overflow com sinal e string sem terminador.
4. Para o `memcmp`: pense no que mais, alem dos valores dos campos, ocupa os bytes de uma struct.
5. Nos sanitizers: rode um exemplo correto e um com defeito conhecido, e anote a diferenca entre "o programa imprimiu o esperado" e "o sanitizer nao reportou nada".
