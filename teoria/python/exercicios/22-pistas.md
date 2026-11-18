---
layout: default
---

# Aula 22 (Python): pistas graduais

Volte aos [exercicios](../22.md#13-exercicios-obrigatorios) antes de abrir as solucoes.

1. Backoff crescente espera mais a cada tentativa: 1, 2, 4... com teto.
2. Job invalido nao merece retry: ele vai para outra fila no primeiro erro de validacao.
3. Retry sem idempotencia repete efeito: enviar o mesmo email duas vezes e o exemplo minimo.
4. Trocar email por relatorio troca a funcao do job, nao a maquina ao redor.
5. Metrica de worker responde: quantos, com que desfecho, em quanto tempo.
