---
layout: default
---

# Aula 17 (Rust): pistas graduais

Volte aos [exercicios](../17.md#14-exercícios-obrigatórios) antes de abrir as solucoes.

1. Sorts: mesma entrada, varias repeticoes, mediana — e confira a saida ordenada antes de cronometrar.
2. AoS contra SoA: some **um campo** nos dois layouts com o mesmo total.
3. False sharing: dois contadores atomicos colados contra dois separados por `repr(align(64))`.
4. Flamegraph: o gargalo nao-obvio se esconde em custo que o fonte nao mostra (clone, hash).
5. SIMD manual por chunks precisa de sobra tratada (o resto fora dos blocos de 8).
6. Layout: some tamanhos e alinhamentos campo a campo antes de medir a struct montada.
7. PGO pede uma carga representativa para treinar; sem ela, o numero nao significa nada.
