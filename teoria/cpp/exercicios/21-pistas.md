---
layout: default
---

# Aula 21 (C++): pistas graduais

Volte aos [exercicios](../21.md#21-exercicios-obrigatorios) antes de abrir as solucoes.

1. `promise` e a ponta que entrega; `future` e a ponta que espera. Uma thread entrega, outra recebe.
2. No pool, tres tarefas pedem tres submissoes e tres `future`s guardados para colher.
3. A excecao vai dentro do pacote: `set_exception` ou deixar a tarefa lancar e o pool capturar.
4. `get()` bloqueia a thread que o chama; dentro do loop, bloquear e parar o mundo.
5. Pool multiplexa **CPU** entre threads; multiplexacao de I/O espera **descritores** numa thread so.
