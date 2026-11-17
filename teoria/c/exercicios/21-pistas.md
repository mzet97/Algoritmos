---
layout: default
---

# Aula 21: pistas graduais

Volte aos [exercicios](../21.md#20-exercicios-obrigatorios) antes de abrir as solucoes.

1. No thread-per-connection, desenhe o que nasce junto com cada `accept` e o que essa entidade faz enquanto o cliente esta ocioso.
2. No event loop, desenhe um unico fluxo que acorda, pergunta quem esta pronto e atende um pouco de cada.
3. Nos custos: pilha por thread, troca de contexto, memoria por conexao ociosa contra buffer por conexao no loop.
4. Mostly idle significa: muitas conexoes, quase nenhuma com trabalho agora — quem paga por conexao parada em cada modelo?
5. No hibrido: o loop cuida do que e rapido e nao bloqueante; o que pode bloquear vai para o pool com fila limitada.
