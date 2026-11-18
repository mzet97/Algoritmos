---
layout: default
---

# Aula 37: soluções comentadas

Compare após tentar os [exercícios](../37.md#7-exercícios-obrigatórios). Variantes sobre `teoria/cpp/kernel37/`, compiladas com as flags do `Makefile` do estágio e rodadas no QEMU.

1. Previsão: **não para** — `TIMER_OK`, `SCHED_OK`, `DONE` normais, com `MPIE = 0` lido no primeiro trap (verificado). A regra de entrega: interrupção de máquina com o hart em modo menos privilegiado trap sempre; `MIE`/`MPIE` só velam interrupções com o hart *em* modo-M. Tirar o `MPIE` do `mstatus` de entrada não cala o timer porque o timer não passa por aquela porta — ele entra pela diferença de privilégio. (O outro lado da regra também foi verificado: com o hart preso em modo-M e `MIE = 0`, o timer cala de verdade.)

2. Diagnóstico: **tempestade, não uma interrupção** (verificado 3/3: ~200 `A`s, zero `B`s, `TIMER_OK` + `SCHED_OK` + `DONE` mesmo assim). Sem rearme, o pedido segue pendente (nível, não borda): cada `mret` re-trapa na hora, 10 trocas acontecem em microssegundos, e a tarefa B restaurada é preemptada antes de imprimir uma letra. O critério (`trocas >= 10`) conta trocas, não cadência — tempestade passa no teste sem escalonar de verdade. A correção da aula registra isso: rearmar é o que dá ritmo ao preemptivo; marcador prova que a regra rodou, não que o ritmo existiu.

3. Projeto: com a tarefa C, giro `(atual + 1) % 3` e limiar de 15 trocas com as três marcadas: A/B/C imprimem ~1000 letras cada (verificado: 1025/1029/1049) e `SCHED_OK` fecha. Mudou: política (giro de 3, limiar 15). Não mudou: quantum igual, sem prioridade, sem bloqueio — round-robin de 3 continua não sendo escalonador de produção. O que o exercício prova: a troca vive inteira no handler em C (salva frame, devolve outro); o `trap.S` só restaura o que lhe entregam — a N-ésima tarefa não exige assembly novo.

4. Previsão: **nenhum `B`, sem `SCHED_OK`** — mas `TIMER_OK` sim (verificado). Salvar A e "trocar" para A conta `trocas`, o timer está provado, mas `tarefas[1].rodou` nunca marca e o critério barra. Trocar com quem nunca executou é trocar com morto: o teste exige as duas vivas exatamente para que "10 trocas" não seja A piscando para si mesmo. Contador sem testemunha de vida é vaidade estatística.
