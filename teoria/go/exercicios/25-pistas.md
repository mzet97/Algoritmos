---
layout: default
---

# Aula 25 (Go): pistas graduais

Volte aos [exercicios](../25.md#21-exercicios-obrigatorios) antes de abrir as solucoes.

1. Log de resiliencia conta a decisao: tentativa N, espera X, circuito abriu.
2. Contagem de falhas/sucessos e estado; `/metrics` so expoe o que o estado guarda.
3. Esperar pela quota com timeout troca rejeicao instantanea por latencia limitada.
4. `429` e `503` pedem retry com espera; `502`/`504` pedem cautela e idempotencia.
5. Escrita so repete com seguranca quando repetir nao duplica: chave de idempotencia.
6. Carga contra dependencia instavel e varias goroutines com relogio — e contadores no fim.
