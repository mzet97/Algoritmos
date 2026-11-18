---
layout: default
---

# Aula 21 (Rust): pistas graduais

Volte aos [exercicios](../21.md#exercícios-obrigatórios) antes de abrir as solucoes.

1. Salas: o mapa de `broadcast::Sender` por nome substitui o `tx` unico; conectar e assinar a sala da rota.
2. Ao trocar de sala (nova conexao na outra rota), a inscricao antiga deve cair — quem guarda o `Receiver`?
3. Heartbeat: na tarefa de envio, `timeout(30s, rx.recv())` estourando vira `Message::Ping`.
4. Cliente que nao responde pong nem le: o que acontece com o canal dele com o tempo?
