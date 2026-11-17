# Pistas da aula 23b

1. Desenhe duas colunas (A e B) e uma linha por evento: `PUT op1`, morte de B, `PUT op2`, lixo no log, reinício, reenvio. Em cada célula escreva o conjunto `{k1, k3}` que aquele nó tem. Onde o teste lê `g6`, o que B pode saber?
2. Sem o selo, o arquivo fica com `{"id":"lixo"PUT op2 k3 30\n` numa linha só. O que o replay faz com linha que não começa com `PUT`? Qual escrita some na segunda reinicialização?
3. `open` com `O_WRONLY` e depois `read`: consulte a página do `open` sobre modo de acesso e o `errno` de ler sem permissão de leitura. Que resposta o cliente recebe e por quê?
4. O encaminhamento tem timeout de 2 s e o atraso é antes do `connect`. Com 5 s de atraso, quem desiste primeiro: o teste (5 s), o encaminhamento (2 s) ou ninguém? O dado chega depois mesmo assim?
