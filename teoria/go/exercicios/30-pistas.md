---
layout: default
---

# Aula 30 (Go): pistas graduais

Volte aos [exercicios](../30.md#22-exercicios-obrigatorios) antes de abrir as solucoes.

1. Weighted round-robin distribui na proporcao dos pesos ao longo de um ciclo completo.
2. Health check HTTP pergunta ao `/healthz` do backend, nao ao soquete dele.
3. Prometheus e texto com convencao: uma linha por metrica, rotulos entre chaves.
4. `log/slog` registra a escolha: backend, duracao e o motivo quando nao ha escolha.
5. Circuito por backend conta falhas seguidas e tira o doente da rotacao ate ele provar saude.
6. `httptest.Server` finge backend saudavel e falho sem sair do teste.
