---
layout: default
---

# Aula 29 (C++): pistas graduais

Volte aos [exercicios](../29.md#21-exercicios-obrigatorios) antes de abrir as solucoes.

1. No `perf stat`, compare ciclos por elemento entre os dois lacos na mesma execucao.
2. Menos iteracoes com passos largos pode tocar mais linhas; conte linhas de 64 bytes, nao passos.
3. No `perf report`, a funcao quente e a de maior amostra — confira em vez de presumir.
4. Callgrind conta instrucoes pela arvore de chamadas; `perf` amostra ciclos reais.
5. `branch-misses` alto pede desvio dependente de dados imprevisiveis no laco.
6. `cache-misses` no stride: cada passo de 64 bytes estreia uma linha e desperdiça o resto.
