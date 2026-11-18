---
layout: default
---

# Aula 37: pistas graduais

Volte aos [exercícios](../37.md#7-exercícios-obrigatórios) antes de abrir as soluções.

1. Sem `MPIE`, tudo passa igual — e o `MPIE` lido no primeiro trap sai **0** (verificado). A regra de entrega: interrupção de máquina com hart em modo menos privilegiado trap sempre; `MIE` só vela interrupções com o hart *em* modo-M. `MPIE`/`MIE` regulam a porta de dentro; de fora (modo-S), a campainha toca de qualquer jeito. (Teste o outro lado: com o hart preso em modo-M e `MIE = 0`, o timer cala de verdade.)
2. Sem rearme não vem "uma interrupção": vem tempestade (verificado: ~200 `A`s, zero `B`s, `TIMER_OK` + `SCHED_OK` + `DONE`). O pedido segue pendente (nível, não borda), cada `mret` re-trapa na hora, e 10 trocas acontecem em microssegundos — o `B` restaurado é preemptado antes de imprimir. O critério conta trocas, não cadência: tempestade passa no teste sem escalonar de verdade. Rearmar é o que dá ritmo; sem ritmo, o teste mente.
3. Três tarefas, giro de 3, 15 trocas: A/B/C imprimem ~1000 letras cada (verificado) e `SCHED_OK` exige as três com `rodou`. O que mudou: política (giro de 3, limiar 15). O que não mudou: quantum igual, sem prioridade, sem bloqueio — round-robin maior continua não sendo escalonador de produção, só prova de troca N-ária.
4. Sem alternar, nenhum `B` aparece (verificado) e `SCHED_OK` nunca sai — mas `TIMER_OK` sim. O contador `trocas` anda (salva A, "troca" para A), o timer está provado, mas `tarefas[1].rodou` nunca marca. O critério exige as duas vivas: trocar com quem nunca executou é trocar com morto, e o teste barra.
