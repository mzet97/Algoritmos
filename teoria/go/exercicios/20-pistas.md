---
layout: default
---

# Aula 20 (Go): pistas graduais

Volte aos [exercicios](../20.md#18-exercicios-obrigatorios) antes de abrir as solucoes.

1. `/version` e um handler JSON comum; o middleware e quem envolve os outros.
2. Middleware de header: sem o header, `401`/`403` (escolha e documente) antes do handler.
3. `MaxBytesReader` corta o corpo no limite; o handler percebe no `Decode` como erro.
4. Metodo incorreto se responde com `405` e header `Allow`.
5. Separar bootstrap/mux/middlewares e montar o mux com as pecas, como na [aula 16](16-solucoes.md).
6. Shutdown gracioso: `srv.Shutdown(ctx)` espera as requisicoes ativas; o sinal so dispara o processo.
