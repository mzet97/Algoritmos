---
layout: default
---

# Aula 28 (C++): pistas graduais

Volte aos [exercicios](../28.md#19-exercicios-obrigatorios) antes de abrir as solucoes.

1. `compact_us` e `wide_us` somam a mesma coisa com geometrias diferentes; confira as somas antes dos tempos.
2. False sharing e coerencia disputada sem dado compartilhado — releia a [aula 28 de C](../../c/exercicios/28-solucoes.md).
3. `alignas(64)` ajuda quando contadores vizinhos escritos por threads diferentes dividem linha.
4. SoA quando a varredura usa poucos campos de muitos registros; AoS quando usa o registro inteiro.
5. Sinais: IPC baixo, `cache-misses` altos e threads que nao escalam apesar de trabalho independente.
