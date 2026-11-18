---
layout: default
---

# Aula 11 (Go): pistas graduais

Volte aos [exercicios](../11.md#13-exercicios-obrigatorios) antes de abrir as solucoes.

1. Cada `wg.Add(1)` pede um `Done()` garantido — `defer wg.Done()` no topo da goroutine.
2. No contador com Mutex, o incremento inteiro fica entre `Lock` e `Unlock` (ou `defer Unlock`).
3. Com `atomic.Int64`, o incremento e `Add(1)`: sem trava, sem esquecer de destravar.
4. Channel faria sentido se o trabalho fosse **passar** valores; mutex, se fosse proteger estado.
5. O exemplo inseguro e o mesmo sem trava nenhuma: o `-race` e o juiz, nao a saida.
