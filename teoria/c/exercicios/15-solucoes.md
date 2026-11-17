---
layout: default
---

# Aula 15: solucoes comentadas

Compare apos tentar os [exercicios](../15.md#17-exercicios-obrigatorios).

1, 2, 3 e 4. Servidor de eco sequencial, protocolo de uma linha terminada em `\n`, porta como argumento:

```c
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int envia_tudo(int fd, const char *buf, size_t n)
{
    size_t enviado = 0;
    while (enviado < n) {
        ssize_t e = send(fd, buf + enviado, n - enviado, 0);
        if (e <= 0) {
            return -1;
        }
        enviado += (size_t)e;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "uso: %s PORTA\n", argv[0]);
        return 2;
    }
    int porta = atoi(argv[1]);
    if (porta <= 0 || porta > 65535) {
        fprintf(stderr, "porta invalida\n");
        return 2;
    }

    int servidor = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor < 0) {
        perror("socket");
        return 1;
    }
    int um = 1;
    setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &um, sizeof um);

    struct sockaddr_in endereco;
    memset(&endereco, 0, sizeof endereco);
    endereco.sin_family = AF_INET;
    endereco.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    endereco.sin_port = htons((unsigned short)porta); /* ordem da rede */

    if (bind(servidor, (struct sockaddr *)&endereco, sizeof endereco) != 0) {
        perror("bind");
        close(servidor);
        return 1;
    }
    if (listen(servidor, 4) != 0) {
        perror("listen");
        close(servidor);
        return 1;
    }

    for (;;) {
        int cliente = accept(servidor, NULL, NULL);
        if (cliente < 0) {
            perror("accept");
            continue;
        }
        char linha[256];
        size_t usadas = 0;
        for (;;) {
            if (usadas == sizeof linha) {
                close(cliente);
                break;
            }
            ssize_t lidos = recv(cliente, linha + usadas,
                                 sizeof linha - usadas, 0);
            if (lidos <= 0) {
                close(cliente);
                break;
            }
            usadas += (size_t)lidos;
            if (memchr(linha, '\n', usadas) != NULL) {
                envia_tudo(cliente, linha, usadas); /* eco da linha */
                close(cliente);
                break;
            }
        }
    }
    return 0;
}
```

A porta chega como texto e vira o campo `sin_port` via `htons`: a rede usa ordem big-endian, e `htons` converte da ordem do hospedeiro — em maquina little-endian ele troca os bytes; em big-endian, e identidade. Trocar a porta do laboratorio e trocar o argumento, nao o codigo.

5. TCP entrega um fluxo ordenado de bytes sem fronteiras de mensagem: dois `send` do cliente podem chegar num `recv` so, e um `send` pode ser repartido em varios `recv`. Quem precisa de mensagens define o enquadramento — aqui, "uma linha ate `\n` com teto de 256 bytes" — e acumula ate o delimitador aparecer, como o servidor acima faz. E essa a base que a [aula 14a](../14a_concorrencia_e_threads.md) e o servidor da aula 31 endurecem.

**Autoverificacao:** dois clientes sequenciais recebem o proprio eco, uma linha enviada em dois `send` separados ainda e ecoada inteira e linha sem `\n` acima do teto encerra a conexao sem travar o servidor.
