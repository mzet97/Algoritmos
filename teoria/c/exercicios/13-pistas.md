---
layout: default
---

# Aula 13: pistas graduais

Volte aos [exercicios](../13.md#16-exercicios-obrigatorios) antes de abrir as solucoes.

1. `peek` le o topo sem mover o indice; reutilize a mesma guarda de pilha vazia do `pop`.
2. Na fila circular, inicio e fim avancam com modulo da capacidade; voce precisa distinguir cheia de vazia (contador de elementos resolve).
3. Remover no inicio da lista e reencadear a cabeca para o segundo no e liberar o antigo; trate lista vazia e de um so elemento.
4. Para a comparacao: pense em quantos bytes uteis cada passo de iteracao traz para o cache em cada estrutura.
5. Para a explicacao: relacione localidade com linhas de cache de 64 bytes e com o custo de perseguir ponteiros.
