---
layout: default
---

# Aula 17 (Python): pistas graduais

Volte aos [exercicios](../17.md#18-exercicios-obrigatorios) antes de abrir as solucoes.

1. `perf_counter` mede parede com resolucao; compare o mesmo trabalho, nao trabalhos parecidos.
2. `timeit` repete e devolve o melhor: e a mesma comparacao sem o seu laco de repeticao.
3. `cProfile` ordena por tempo acumulado: o hotspot e quem esta no topo, nao quem voce suspeita.
4. Generator troca memoria por preguiça: a lista inteira some, o item aparece um por vez.
5. `__slots__` troca o `__dict__` por campos fixos — investigue tamanho e o que se perde.
