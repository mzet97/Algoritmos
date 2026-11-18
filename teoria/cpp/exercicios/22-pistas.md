---
layout: default
---

# Aula 22 (C++): pistas graduais

Volte aos [exercicios](../22.md#24-exercicios-obrigatorios) antes de abrir as solucoes.

1. Na ordem das impressoes: coroutine comeca so no primeiro `co_await`/`co_yield` que suspende de verdade.
2. O frame guarda o que precisa sobreviver a suspensao: parametros, locais vivas e o ponto de retomada.
3. `future.get()` espera bloqueando; `co_await` suspende a coroutine e devolve a thread.
4. "Linear" e a **leitura** do codigo; a execucao pode interlevar com outras coroutines em cada suspensao.
5. Ownership em coroutine: quem destroi o frame, o que as referencias capturadas apontam quando retomar, quem cancela.
