---
layout: default
---

# Aula 16 (Python): pistas graduais

Volte aos [exercicios](../16.md#19-exercicios-obrigatorios) antes de abrir as solucoes.

1. `gather` espera as duas coroutines como quem espera threads: juntas.
2. I/O-bound em asyncio troca thread por coroutine quando a espera e de rede/disco assincrono.
3. `time.sleep` dentro de coroutine congela o loop inteiro; `asyncio.sleep` so congela ela.
4. `wait_for` corta a tarefa pelo relogio e levanta `TimeoutError`.
5. `asyncio.Lock` protege trecho entre `await`s, onde outra coroutine pode entrar.
