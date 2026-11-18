---
layout: default
---

# Aula 20 (Rust): pistas graduais

Volte aos [exercicios](../20.md#exercícios-obrigatórios) antes de abrir as solucoes.

1. O historico guarda os **ultimos 5**: ao criar, empurre e remova o excedente da frente; leia com `read()`, sem `.await` segurando a guarda.
2. JSON de saida e um `Vec<String>` serializado com `Json`.
3. Rate limit por IP: o IP vem de `ConnectInfo<SocketAddr>`; o contador mora num `Mutex<HashMap<_, u8>>` no estado.
4. `429` e uma resposta HTTP comum: devolva `(StatusCode::TOO_MANY_REQUESTS, ...)` ao passar de 5.
