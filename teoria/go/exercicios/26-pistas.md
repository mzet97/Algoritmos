---
layout: default
---

# Aula 26 (Go): pistas graduais

Volte aos [exercicios](../26.md#22-exercicios-obrigatorios) antes de abrir as solucoes.

1. Enfileirar com timeout e `select` entre o canal da fila e `time.After`.
2. `log/slog` com campos nomeados por evento: scheduler, worker, retry, dead letter.
3. Persistir conclusao pode comecar simples: uma linha por job num arquivo, `fsync` na conclusao.
4. Job caro ocupa worker por mais tempo: o backlog cresce na mesma taxa de chegada.
5. Dead letter e outra fila com comando de reprocessamento, nao um cemiterio.
6. Sobreviver a crash pede o estado fora da memoria: fila e conclusoes persistidas.
