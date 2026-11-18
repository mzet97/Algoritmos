---
layout: default
---

# Aula 09 (Go): pistas graduais

Volte aos [exercicios](../09.md#13-exercicios-obrigatorios) antes de abrir as solucoes.

1. Dobrar via ponteiro escreve em `*p`, nao numa copia do numero.
2. No `Contador`, o metodo que incrementa precisa de receiver `*Contador`.
3. Por valor, a funcao ganha uma copia da struct; por ponteiro, o original.
4. `new(int)` devolve `*int` apontando para o zero value.
5. No escape analysis, procure "escapes to heap" e relacione com quem guarda o ponteiro depois do retorno.
