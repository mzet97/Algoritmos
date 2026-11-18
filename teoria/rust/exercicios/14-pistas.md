---
layout: default
---

# Aula 14 (Rust): pistas graduais

Volte aos [exercicios](../14.md#15-exercícios-obrigatórios) antes de abrir as solucoes.

1. Fetch concorrente simulado: `tokio::join!` ou tarefas com `spawn`; o tempo total vira o maior, nao a soma.
2. No pipeline async, cada estagio e uma task ligada por `mpsc`; feche o canal para encerrar a cadeia.
3. Echo com timeout: `tokio::select!` entre ler do socket e o ramo de tempo.
4. Semaforo de 3 permissoes: adquira **antes** do download e devolva ao soltar a permissao.
5. Salas de chat: um `broadcast::Sender` por sala num mapa compartilhado; entrar e trocar a inscricao.
6. Backpressure: canal com capacidade 5 faz o produtor esperar no `send().await` — e isso que voce deve observar.
7. Crawler: fila de URLs, conjunto de visitadas e semaforo entre a fila e o "download".
