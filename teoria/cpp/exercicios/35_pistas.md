---
layout: default
---

# Aula 35: pistas graduais

Volte aos [exercícios](../35.md#7-exercícios-obrigatórios) antes de abrir as soluções.

1. 8 GiB cai em `FALTA_INESPERADA mtval=0x200000000` + desligar `0x3333`. A condição `tval == VA_FALTA` é o contrato: demanda atende o endereço combinado, o resto é defeito do experimento. Mapear qualquer falta seria dar ao programa o poder de inventar memória — demanda serve, não inventa.
2. Sem `W`, a escrita falta, o handler mapeia R-only e reexecuta — e a escrita falta *de novo*, agora com `demanda_ok == 1`: `FALTA_INESPERADA`. O `DEMAND_OK` saiu, mas a permissão estava errada: mapear sem o direito certo é adiar a falta de uma instrução. A guarda de estado impede o loop infinito de remapear o mesmo endereço.
3. A segunda condição checa `tval == VA_FALTA2 && demanda_ok == 1`: ordem importa — o contador virou máquina de estados (0→1→2). `DEMAND2_OK` sai, a leitura confere, o `DONE` fecha. Remapear o mesmo endereço duas vezes bateria na guarda; cada endereço tem sua vez, cada vez seu marcador.
4. Um `DEMAND_OK` só (verificado): a segunda escrita acerta direto, `SEM_NOVA_FALTA`. O `sfence.vma` instalou a tradução no TLB para os acessos seguintes — não foi carimbo de uma instrução. Demanda mapeia uma vez e serve para sempre (até invalidação): falta é evento de instalação, não pedágio por acesso.
