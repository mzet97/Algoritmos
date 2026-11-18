---
layout: default
---

# Aula 15 (Go): pistas graduais

Volte aos [exercicios](../15.md#14-exercicios-obrigatorios) antes de abrir as solucoes.

1. O trabalhador observa `ctx.Done()` no `select` junto do canal de trabalho.
2. `WithTimeout` e `WithCancel` + um relogio: a funcao devolve o contexto e o `cancel`.
3. Ao sair por cancelamento, devolva `ctx.Err()` — quem chamou distingue timeout de cancelamento manual.
4. `ctx` e o primeiro parametro, antes de qualquer outro, e nunca mora dentro de struct.
5. Ignorar contexto em backend: a requisicao morreu, o trabalho continua — onde isso aparece na conta?
