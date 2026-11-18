---
layout: default
---

# Aula 11b: pistas graduais

Volte aos [exercícios](../11b_cancelamento_e_gomaxprocs.md#6-exercícios-obrigatórios) antes de abrir as soluções.

1. Sem `WithCancel`, ninguém sinaliza: os 2000 concluem e o `panic` confirma (verificado: `2000 concluidos de 2000`). Pool cooperativo obedece a sinal — sem sinal, obedece ao fim da fila. Cancelamento em Go é pedido, não ordem; sem pedido, o laço é o programa.
2. Sem `Done` no worker e sem `close`, cada worker dorme em `<-jobs` para sempre e o `wg.Wait` nunca volta (verificado: 25 s sem uma linha, `timeout` mata). O `range` do feeder acaba (lista finita), mas os workers não sabem: `close` ou `Done` — um dos dois precisa existir. Exercício bônus: tire só o `close` e prove que passa (o `Done` esvazia); tire só o `Done` e prove que passa (o `close` esvazia).
3. Com `WithTimeout` de 50 ms e job de 1 ms: ~379 concluem, o resto nem começa (verificado) — sem `cancela()` explícito em lugar nenhum. Prazo é cancelamento com relógio em vez de vigia: o `ctx.Done()` fecha sozinho. A faixa do exemplo (500–1999) foi calibrada para jobs rápidos; com jobs lentos ela cai — calibre a faixa ao trabalho, não o trabalho à faixa.
4. `GOMAXPROCS=N` vence (verificado nas duas máquinas de referência) porque somar é CPU-bound: 1 núcleo faz tudo em fila, N fazem em paralelo. O `-race` instrumenta cada acesso (ordem de grandeza mais lento) — comparar tempos sob `-race` mede o detector, não o programa. Benchmark honesto: binário limpo, como o exemplo imprime.
