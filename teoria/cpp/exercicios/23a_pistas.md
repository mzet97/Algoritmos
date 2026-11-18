---
layout: default
---

# Pistas da aula 23a

1. Desenhe o caminho de um `PING` do fio até a resposta marcando onde cada chamada pode devolver `EAGAIN` e o que o loop faz em cada caso em vez de travar.
2. Reduza `SAIDA_MAX` para 64 e conecte um cliente que nunca lê. Observe quem cai e quem continua respondendo — e onde o teto morde.
3. Suba `MAX_CLIENTES` para 200 e abra 300 conexões de uma vez; conte quantas viram `ECONNREFUSED`, quantas recebem `PONG` e explique cada grupo pelo backlog, pelo teto e pelo `ulimit`.
4. Envie 1 MB numa linha e cronometre o EOF. Depois troque o teste para enviar 1 MB *sem* `\n` e segurar a conexão aberta: o que muda no servidor e por quê?
