---
layout: default
---

# Aula 31 - Solução comentada e rubrica

[aula31.c](../exemplos/aula31.c) e a implementacao de referencia. O contrato do servidor inclui bind em 127.0.0.1, sockets nao bloqueantes, parser por linha, saida limitada, envio parcial e MSG_NOSIGNAL. A leitura e a escrita fazem no maximo uma chamada de I/O por cliente em cada rodada. EAGAIN deixa o estado intacto para a proxima notificacao.

**Modelo de estados:** aberto aceita entrada e pode ter saida pendente; fechando nao aceita novos comandos, mas escoa saida pendente; fechado libera o descritor e o slot. QUIT e EOF movem para fechando. Linha longa, NUL, erro de I/O e fila cheia movem para fechado. O servidor nao garante timeout e nao oferece autenticacao ou TLS.

**Cenarios de aceite:** PING fragmentado, comandos agrupados, linha longa, NUL, fechamento durante envio e leitor lento junto de outro cliente. O teste deve observar resposta e EOF, nao apenas que o processo continua rodando. A medicao de latencia do segundo cliente requer repeticoes e anotacao de hardware, sistema e carga.

## Rubrica de 10 pontos

- 3: protocolo e estados corretos em fragmentacao, coalescencia e QUIT.
- 2: sockets nao bloqueantes e escrita parcial sem travar outro cliente.
- 2: limites por cliente e memoria agregada explicados.
- 2: testes de falha reproduziveis e modelo de ameacas.
- 1: medicao, comunicacao de limites e limpeza de recursos.
