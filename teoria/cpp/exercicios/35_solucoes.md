---
layout: default
---

# Aula 35: soluções comentadas

Compare após tentar os [exercícios](../35.md#7-exercícios-obrigatórios). Variantes sobre `teoria/cpp/kernel35/`, compiladas com o `Makefile` do estágio e rodadas no QEMU.

1. Previsão: `FALTA_INESPERADA mtval=0x0000000200000000` + desligar `0x3333` (verificado). A condição `tval == VA_FALTA && !demanda_ok` é o contrato inteiro: demanda atende o endereço combinado no estado combinado. Qualquer outro endereço é defeito do experimento — e "mapear qualquer falta" seria um allocator sob demanda, não paginação sob demanda: o handler decidiria *onde* há memória em vez de *quando* instalá-la.

2. Diagnóstico: `DEMAND_OK` sai — e em seguida `FALTA_INESPERADA mtval=0x100000000` (verificado). A sequência: escrita falta (causa 15) → handler mapeia R-only → `sfence` → `mepc` intacto → reexecuta a escrita → falta de novo (sem W) → `demanda_ok` já é 1 → barra → `0x3333`. Mapear sem o direito certo adia a falta de exatamente uma instrução. A guarda de estado (`demanda_ok`) é o que impede o loop infinito de remapear-e-faltar; sem ela, o kernel giraria mudo entre falta e remapeamento.

3. Projeto: com `VA_FALTA2` (5 GiB) e a segunda condição (`tval == VA_FALTA2 && demanda_ok == 1` → 2): `DEMAND_OK`, `DEMAND2_OK`, `DONE` (verificado). O contador virou máquina de estados 0→1→2: cada endereço tem sua vez, cada vez seu marcador. O que isso impede: remapear o mesmo endereço duas vezes (a segunda falta no mesmo `tval` não casa nenhuma condição e cai em `FALTA_INESPERADA`). Demanda com N endereços é N cláusulas ordenadas — e a ordem é o protocolo.

4. Previsão: **um** `DEMAND_OK` (verificado: `DEMAND_OK`, `SEM_NOVA_FALTA`, `DONE`). A segunda escrita (0xBEEF) acerta direto no mapeamento instalado. O `sfence.vma` não carimbou uma instrução: instalou a tradução no TLB para todos os acessos seguintes. Falta é evento de instalação, não pedágio por acesso — e é por isso que demanda escala: o custo paga-se uma vez por página, não uma vez por toque.
