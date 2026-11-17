---
layout: default
---

# Aula 30: pistas graduais

Volte aos [exercicios](../30.md#21-exercicios-obrigatorios) antes de abrir as solucoes.

1. Vetorizacao gosta de laco contavel, sem dependencias entre iteracoes e sem desvios que dependam dos dados.
2. `-fopt-info-vec` responde pelo compilador: procure "vectorized loops" e as razoes de recusa.
3. `-march=native` libera as instrucoes da sua CPU; anote que o binario deixa de ser portavel para CPUs mais antigas.
4. SoA ajuda SIMD pelo mesmo motivo que ajuda cache: elementos consecutivos do mesmo campo.
5. `restrict` e uma promessa sua sobre aliasing; pense no que o compilador pode assumir com ela.
6. Intrinsicos se justificam onde a medicao mostra que o autovetorizador nao chega, nao por esporte.
