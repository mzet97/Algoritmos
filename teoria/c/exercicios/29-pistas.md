---
layout: default
---

# Aula 29: pistas graduais

Volte aos [exercicios](../29.md#22-exercicios-obrigatorios) antes de abrir as solucoes.

1. No `perf stat`, compare os contadores dos dois lacos na mesma execucao e com o mesmo total somado.
2. Menos iteracoes com passos largos pode tocar mais linhas de cache que mais iteracoes contiguas; conte linhas, nao passos.
3. `perf record` + `perf report`: a funcao quente e a de maior porcentagem de amostras, nao a maior do fonte.
4. No callgrind, o custo se distribui pela arvore de chamadas; compare a forma, nao so o total.
5. `branch-misses` alto pede um desvio dependente de dados imprevisiveis dentro do laco.
6. Para `cache-misses` no stride: relacione o passo com o tamanho da linha.
