---
layout: default
---

# Aula 34: pistas graduais

Volte aos [exercícios](../34.md#7-exercícios-obrigatórios) antes de abrir as soluções.

1. `0x00000000` também não decodifica — e o `mtval` imprime `0x0000000000000000` em vez de `0x00000000ffffffff`. O `mtval` não diz "ilegal" em abstrato: ele fotografa os bits que o decodificador recusou. Marcador sem dado prova que algo aconteceu; marcador com dado prova *o quê* — a aula imprime o valor por esse motivo.
2. Sem `+= 4`, o `mret` volta ao mesmo `ecall`: `TRAP_ECALL_OK` sai uma vez (a guarda `ecall_ok`), depois o loop gira mudo até o `timeout` — sem `DONE`, sem ilegal. `mepc` parado é reexecução: `ecall` pede +4 (chamada atendida, segue o programa), falta de página pede +0 (reexecuta o acesso agora mapeado). Trocar as duas quebras as duas retomadas.
3. O assembler emitiu `ebreak` como `c.ebreak` (2 bytes, `0x9002`): `+= 4` cego pula para o meio da `.word` seguinte — e o `mtval` entrega `0x678dffff` em vez de `0xFFFFFFFF`. A correção lê a meia-palavra no `mepc` e avança 2 ou 4 pelos bits 1:0. Com o avanço certo, o `mtval` volta a `0xFFFFFFFF` e o `DONE` fecha: a retomada mede, não chuta.
4. Não: a guarda `if (!ecall_ok)` imprime uma vez. Marcador como testemunha única evita poluir a serial em loop real (timer, por exemplo, bateria milhares). Contador faria falta onde a quantidade importa — reentrância (trap dentro de trap) ou estatística de interrupções. Aqui, uma testemunha basta porque o experimento é linear.
