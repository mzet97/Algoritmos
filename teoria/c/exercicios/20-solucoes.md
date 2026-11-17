---
layout: default
---

# Aula 20: solucoes comentadas

Compare apos tentar os [exercicios](../20.md#17-exercicios-obrigatorios).

1, 2 e 3. Servidor de eco multi-cliente com `poll`, sockets nao bloqueantes e `EAGAIN` tratado — a versao com `select` segue a mesma estrutura trocando o mecanismo de espera:

```c
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_CLIENTES 8
#define TAM_LINHA 256

struct Cliente {
    int fd;
    char buf[TAM_LINHA];
    size_t usadas;
};

static int torna_nao_bloqueante(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        return -1;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "uso: %s PORTA\n", argv[0]);
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
    endereco.sin_port = htons((unsigned short)atoi(argv[1]));
    if (bind(servidor, (struct sockaddr *)&endereco, sizeof endereco) != 0 ||
        listen(servidor, 8) != 0 || torna_nao_bloqueante(servidor) != 0) {
        perror("preparacao");
        close(servidor);
        return 1;
    }

    struct Cliente clientes[MAX_CLIENTES];
    for (int i = 0; i < MAX_CLIENTES; i++) {
        clientes[i].fd = -1;
    }

    for (;;) {
        struct pollfd fds[MAX_CLIENTES + 1];
        fds[0].fd = servidor;
        fds[0].events = POLLIN;
        for (int i = 0; i < MAX_CLIENTES; i++) {
            fds[i + 1].fd = clientes[i].fd;
            fds[i + 1].events = POLLIN;
        }
        if (poll(fds, MAX_CLIENTES + 1, -1) < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("poll");
            break;
        }

        if (fds[0].revents & POLLIN) {
            for (;;) {
                int novo = accept(servidor, NULL, NULL);
                if (novo < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        break; /* fila de aceite esvaziada */
                    }
                    perror("accept");
                    break;
                }
                torna_nao_bloqueante(novo);
                int colocado = 0;
                for (int i = 0; i < MAX_CLIENTES; i++) {
                    if (clientes[i].fd < 0) {
                        clientes[i].fd = novo;
                        clientes[i].usadas = 0;
                        colocado = 1;
                        break;
                    }
                }
                if (!colocado) {
                    close(novo); /* teto de clientes deste laboratorio */
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTES; i++) {
            if (clientes[i].fd < 0 || !(fds[i + 1].revents & POLLIN)) {
                continue;
            }
            ssize_t lidos = recv(clientes[i].fd,
                                 clientes[i].buf + clientes[i].usadas,
                                 TAM_LINHA - clientes[i].usadas, 0);
            if (lidos == 0) {
                close(clientes[i].fd);
                clientes[i].fd = -1;
                continue;
            }
            if (lidos < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    continue; /* readiness avisou, dados ainda nao */
                }
                close(clientes[i].fd);
                clientes[i].fd = -1;
                continue;
            }
            clientes[i].usadas += (size_t)lidos;
            if (memchr(clientes[i].buf, '\n', clientes[i].usadas) != NULL) {
                size_t enviado = 0;
                while (enviado < clientes[i].usadas) {
                    ssize_t e = send(clientes[i].fd,
                                     clientes[i].buf + enviado,
                                     clientes[i].usadas - enviado, 0);
                    if (e <= 0) {
                        break;
                    }
                    enviado += (size_t)e;
                }
                close(clientes[i].fd);
                clientes[i].fd = -1;
            } else if (clientes[i].usadas == TAM_LINHA) {
                close(clientes[i].fd); /* linha acima do teto */
                clientes[i].fd = -1;
            }
        }
    }
    close(servidor);
    return 0;
}
```

Com `select`, o mesmo desenho usa um `fd_set` reconstruido a cada volta e tem o limite historico de `FD_SETSIZE` descritores; a estrutura (esperar → aceitar → acumular por cliente) nao muda.

4. `select` e `poll` respondem "quais destes fds estao prontos" reconstruindo a pergunta a cada chamada — custo proporcional ao numero de fds, e `select` ainda tem teto de fds. `epoll` inverte: voce registra o interesse uma vez e o kernel devolve so os eventos prontos, o que escala para dezenas de milhares de conexoes majoritariamente ociosas. Os tres entregam **prontidao**, nao dados.

5. Readiness diz "ha bytes para ler", nao "ha uma mensagem completa": a linha pode vir partida em dois eventos, ou duas linhas num evento so. Por isso cada cliente mantem seu buffer e seu `usadas` acima — o enquadramento (aqui, `\n` com teto) e responsabilidade do protocolo, como na [aula 15](15-solucoes.md); o mecanismo de espera so agenda quem merece atencao.

**Autoverificacao:** dois clientes **simultaneos** recebem o proprio eco sem esperar um ao outro, desconexao libera a vaga e `EAGAIN` nunca encerra o servidor.
