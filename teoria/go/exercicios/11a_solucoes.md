---
layout: default
---

# Soluções comentadas da aula 11a

## 1. Quatro atos de happens-before

- `go f()`: tudo antes do `go` é visível dentro de `f`.
- `ch <- v` / `<-ch`: escritas antes do envio são visíveis após o recebimento do mesmo valor.
- `mu.Unlock()` / `mu.Lock()`: escritas antes do `Unlock` são visíveis após o próximo `Lock` que o adquire.
- `wg.Add(1)` / `wg.Wait()` retornando: tudo antes de cada `Add` pareado é visível após o `Wait`.

## 2. Cancelamento cooperativo

```go
done := make(chan struct{})
// worker:
select {
case <-done:
    return
case j, ok := <-jobs:
    if !ok {
        return
    }
    total.Add(int64(j * j))
}
// produtor, após N envios:
close(done)
```

Fechar `done` desbloqueia todos os workers ao mesmo tempo; jobs já retirados terminam, jobs na fila ficam. A invariante do total exato cai — e é por isso que o enunciado pede medir perdidos vs. concluídos: cancelamento troca exatidão por prontidão, explicitamente.

## 3. WARNING do `-race` (trecho típico)

```text
WARNING: DATA RACE
Write at 0x... by goroutine N: main.(*Contador).Inc()
Previous write at 0x... by goroutine M: main.(*Contador).Inc()
```

Duas escritas em `c.n` sem sincronização. Restaurar o `Mutex` silencia o relatório porque toda escrita passa a acontecer sob exclusão — e a leitura em `Valor()` também, ou a leitura correria com as escritas.

## 4. Buffer zero e teto da fila

Com `make(chan int)`, o produtor só avança quando um worker recebe: produtor e pool andam amarrados. O teto adequado desacopla rajadas do ritmo dos workers sem esconder backlog infinito: `len(trabalhos)` (aqui, 300) aceita a rajada inteira; sob carga contínua real, teto fixo pequeno + descarte ou backpressure explícita, nunca fila que cresce sem limite.
