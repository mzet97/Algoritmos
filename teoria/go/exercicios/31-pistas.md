---
layout: default
---

# Aula 31 (Go): pistas graduais

Volte aos [exercicios](../31.md#20-exercicios-obrigatorios) antes de abrir as solucoes.

1. Config JSON e um arquivo que se le uma vez e valida antes de qualquer check.
2. Severidade vira codigo de saida: `critical` conta mais que `warning` na hora de sair.
3. Retry so para GET idempotente — e com backoff, nao em rajada.
4. Formato Prometheus e uma funcao de renderizacao sobre os mesmos resultados.
5. `httptest` saudavel responde rapido; o lento dorme mais que o timeout do cliente.
6. Benchmark do runner varia workers e checks separados: um de cada vez.
