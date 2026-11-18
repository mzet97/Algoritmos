---
layout: default
---

# Aula 07 (Go): pistas graduais

Volte aos [exercicios](../07.md#13-exercicios-obrigatorios) antes de abrir as solucoes.

1. Erro e valor de retorno: negativo vira `errors.New` ou `fmt.Errorf`, nunca `panic`.
2. Contexto se adiciona com `%w` para a cadeia continuar inspecionavel com `errors.Is/As`.
3. `defer` empilha: o ultimo registrado e o primeiro a rodar.
4. `recover` so funciona dentro de funcao adiada, e so na borda onde voce sabe o que fazer com o panic.
5. Classifique pelo contrato: entrada invalida esperada e erro; invariante quebrada pelo programador e panic.
