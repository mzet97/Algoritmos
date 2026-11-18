---
layout: default
---

# Aula 23 - Pistas graduais

1. Nao associe uma chamada de recv a uma mensagem. Teste duas mensagens em uma leitura e uma mensagem em duas leituras.
2. Guarde bytes pendentes por conexao. Ao encontrar LF, processe a linha e continue no mesmo bloco.
3. Antes de anexar um byte, confira a capacidade. Antes de alocar um payload anunciado, confira o limite do protocolo.
4. Decida o que fazer com NUL, EOF no meio de uma mensagem e cliente que nunca completa a linha.
5. Para backpressure, limite a fila de saida e escolha entre pausar leitura e fechar conexao.
