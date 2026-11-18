---
layout: default
---

# Aula 11b: soluções comentadas

Compare após tentar os [exercícios](../11b_cancelamento_e_gomaxprocs.md#6-exercícios-obrigatórios). Experimentos sobre `teoria/go/exemplos/11b_cancelamento.go` com `go vet` + `go run -race`.

Nota sobre o exemplo: a primeira versão do worker não tinha custo simulado, e o feeder descarregava os 2000 antes do vigia (1 ms) notar os 500 — o teste falhava ~1 vez em 7 com `2000 concluidos` (feeder venceu a corrida). O `Sleep` de 50 µs por job fixa o cancelamento em ~515 (verificado 5/5): disciplina, não sorte. Se o seu `parados` sair da faixa, suspeite do escalonador antes de suspeitar do código — mas com a faixa calibrada ao trabalho, a suspeita some.

1. Previsão: **2000 de 2000 + `panic: cancelamento nao obedeceu`** (verificado). Sem `WithCancel`, ninguém fecha o contexto: o feeder envia tudo, os workers drenam tudo, e o `parados >= 2000` dispara. Pool cooperativo obedece a sinal — sem sinal, obedece ao fim da fila. Cancelamento em Go é pedido, não ordem; sem pedido, o laço é o programa inteiro.

2. Diagnóstico: **trava muda** — 25 s sem uma linha, `timeout` mata (verificado, `exit 124`). O `range` do feeder acaba (lista finita), mas os 8 workers dormem em `<-jobs` para sempre e o `wg.Wait` nunca volta. Sem `close` e sem `Done`, ninguém solta ninguém. A recíproca também foi verificada: tirando só o `close`, passa (o `Done` esvazia); o `close` aqui é cinto, o `Done` é suspensório — e o exercício pede os dois fora para mostrar o que cada um segurava. (Detalhe que o teste revelou: `break` dentro do `select` quebra o `select`, não o `for` — o feeder termina porque a lista acaba, não porque o `break` o salvou.)

3. Projeto: com `WithTimeout` de 50 ms e job de 1 ms, **~379 concluem** e o resto nem começa (verificado) — sem `cancela()` explícito em lugar nenhum. Prazo é cancelamento com relógio em vez de vigia: o `ctx.Done()` fecha sozinho aos 50 ms. A faixa do exemplo (500–1999) foi calibrada para jobs de 50 µs; com jobs de 1 ms ela cai para ~379 — calibre a faixa ao trabalho, não o trabalho à faixa.

4. Previsão: **`GOMAXPROCS=24` vence** (verificado 3/3: ~915 µs vs ~400 µs): somar é CPU-bound, 1 núcleo faz tudo em fila, 24 fazem em paralelo (2,3×, não 24× — `atomic.Add` serializa no barramento, e memória-bound não escala com núcleo). O `-race` instrumenta cada acesso atômico e sincronização — ordem de grandeza mais lento: comparar tempos sob `-race` mede o detector, não o programa. Benchmark honesto usa binário limpo, como o exemplo imprime — e a aula proíbe citar tempo sob `-race` como tabela exatamente por isso.
