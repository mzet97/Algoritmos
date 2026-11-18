---
layout: default
---

# Aula 34: soluções comentadas

Compare após tentar os [exercícios](../34.md#7-exercícios-obrigatórios). Variantes sobre `teoria/cpp/kernel34/`, compiladas com o `Makefile` do estágio e rodadas no QEMU (`virt`, `-bios none`, `-smp 1`).

1. Previsão: `mtval=0x0000000000000000` (verificado). `0x00000000` também é ilegal (nenhum opcode válido), e o `mtval` fotografa os bits recusados — zeros aqui, `FFFFFFFF` no original. O `mtval` não é um código de erro: é a evidência. É por isso que a aula imprime o valor em vez de só anunciar o marcador.

2. Diagnóstico: `TRAP_ECALL_OK` sai **uma vez**, depois silêncio até o `timeout` — sem `DONE`, sem ilegal (verificado). Sem `+= 4`, o `mret` retorna ao mesmo `ecall`, que traps de novo, para sempre; a guarda `ecall_ok` cala as repetições. `mepc` parado é reexecução da mesma instrução: `ecall` atendido pede +4 (segue o programa), falta de página pede +0 (reatenta o acesso agora mapeado). A retomada mora no número, não no gesto.

3. Projeto: o assembler emite `ebreak` como `c.ebreak` (2 bytes, `0x9002` — confirmado no `objdump`). Com `+= 4` cego, o `mret` cai no meio da `.word` seguinte e o `mtval` entrega `0x678dffff` (verificado) — o salto errado fotografado pelo hardware. A correção mede antes de avançar:

```cpp
case 3: {
    uint16_t meia = *(volatile uint16_t *)tf->f[F_MEPC];
    uint64_t tam = ((meia & 3) == 3) ? 4 : 2;
    tf->f[F_MEPC] += tam;
    return tf;
}
```

Com o avanço pelo tamanho real: `TRAP_EBREAK_OK tam=2`, `mtval=0xFFFFFFFF` intacto, `DONE` (verificado). A regra geral que o exercício revela: retomada = comprimento da instrução que causou o trap — e instrução RISC-V mede-se nos bits 1:0, não se chuta em 4.

4. Previsão: **não** — `TRAP_ECALL_OK` sai uma vez (verificado: dois `ecall`s, um marcador, `DONE` normal). A guarda `if (!ecall_ok)` faz do marcador testemunha única. Em experimento linear, basta; em sistema real com timer, milhares de repetições poluiriam a serial — e é por isso que witness-once é o padrão. Contador faria falta onde quantidade é evidência: reentrância (trap durante trap), taxa de interrupções, estatística de faltas. Aqui, uma testemunha prova o caminho; o resto seria ruído.
