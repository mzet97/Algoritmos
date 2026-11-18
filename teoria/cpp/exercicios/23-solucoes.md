---
layout: default
---

# Aula 23 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../23.md#20-exercicios-obrigatorios). O servidor endurecido completo, com testes adversariais, e a ponte [23a](../23a.md); aqui vai o recorte dos exercicios sobre o servidor simples:

1, 2, 3 e 4. Servidor de linha com `ECHO`, teto de entrada e fila de saida:

```cpp
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <deque>
#include <fcntl.h>
#include <iostream>
#include <poll.h>
#include <string>
#include <unistd.h>
#include <vector>

namespace {

constexpr std::size_t MAX_ENTRADA = 256;
constexpr int MAX_CLIENTES = 8;

int nao_bloqueante(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    return flags < 0 ? -1 : fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

struct Conexao {
    int fd = -1;
    std::string entrada;
    std::deque<std::string> saida;
    std::size_t enviado_atual = 0;
};

std::string despacha(const std::string& linha)
{
    if (linha == "PING") {
        return "PONG\n";
    }
    if (linha.rfind("ECHO ", 0) == 0) {
        return linha.substr(5) + "\n";
    }
    if (linha == "SAIR") {
        return "ADEUS\n";
    }
    return "ERRO comando desconhecido\n";
}

} // namespace

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "uso: " << argv[0] << " PORTA\n";
        return 2;
    }
    int servidor = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor < 0) {
        perror("socket");
        return 1;
    }
    int um = 1;
    setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &um, sizeof um);

    sockaddr_in endereco{};
    endereco.sin_family = AF_INET;
    endereco.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    endereco.sin_port = htons(static_cast<unsigned short>(std::stoi(argv[1])));
    if (bind(servidor, reinterpret_cast<sockaddr*>(&endereco), sizeof endereco) != 0 ||
        listen(servidor, 8) != 0 || nao_bloqueante(servidor) != 0) {
        perror("preparacao");
        close(servidor);
        return 1;
    }

    std::vector<Conexao> conexoes(MAX_CLIENTES);
    for (;;) {
        std::vector<pollfd> fds;
        fds.push_back({servidor, POLLIN, 0});
        for (const auto& c : conexoes) {
            if (c.fd >= 0) {
                short eventos = POLLIN;
                if (!c.saida.empty()) {
                    eventos = static_cast<short>(eventos | POLLOUT);
                }
                fds.push_back({c.fd, eventos, 0});
            }
        }
        if (poll(fds.data(), fds.size(), -1) < 0) {
            if (errno == EINTR) continue;
            perror("poll");
            break;
        }
        if (fds[0].revents & POLLIN) {
            for (;;) {
                int novo = accept(servidor, nullptr, nullptr);
                if (novo < 0) break;
                nao_bloqueante(novo);
                bool colocado = false;
                for (auto& c : conexoes) {
                    if (c.fd < 0) {
                        c = Conexao{};
                        c.fd = novo;
                        colocado = true;
                        break;
                    }
                }
                if (!colocado) close(novo);
            }
        }
        std::size_t indice = 1;
        for (auto& c : conexoes) {
            if (c.fd < 0) continue;
            short rev = fds[indice++].revents;
            bool fechar = false;
            if (rev & POLLIN) {
                char buf[128];
                ssize_t lidos = recv(c.fd, buf, sizeof buf, 0);
                if (lidos == 0) {
                    fechar = true;
                } else if (lidos > 0) {
                    c.entrada.append(buf, static_cast<std::size_t>(lidos));
                    if (c.entrada.size() > MAX_ENTRADA &&
                        c.entrada.find('\n') == std::string::npos) {
                        fechar = true; // teto de entrada excedido
                    }
                    std::size_t pos;
                    while (!fechar &&
                           (pos = c.entrada.find('\n')) != std::string::npos) {
                        std::string linha = c.entrada.substr(0, pos);
                        c.entrada.erase(0, pos + 1);
                        c.saida.push_back(despacha(linha));
                        if (linha == "SAIR") fechar = true;
                    }
                } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
                    fechar = true;
                }
            }
            if (!fechar && (rev & POLLOUT) && !c.saida.empty()) {
                const std::string& atual = c.saida.front();
                ssize_t e = send(c.fd, atual.data() + c.enviado_atual,
                                 atual.size() - c.enviado_atual, 0);
                if (e > 0) {
                    c.enviado_atual += static_cast<std::size_t>(e);
                    if (c.enviado_atual == atual.size()) {
                        c.saida.pop_front();
                        c.enviado_atual = 0;
                    }
                } else if (e < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
                    fechar = true;
                }
            }
            if (fechar) {
                close(c.fd);
                c = Conexao{};
            }
        }
    }
    close(servidor);
    return 0;
}
```

4. Progresso parcial e normal porque `send` e `recv` contratam "o que coube agora", nao "a mensagem inteira": o servidor guarda deslocamentos (`enviado_atual`, restos em `entrada`) e continua na proxima prontidao. Um servidor que supoe atomicidade funciona no laboratorio local e quebra com clientes lentos e mensagens grandes — os testes da [23a](../23a.md) forcam exatamente esses casos.

5. Estado por conexao num servidor real: fd,.buffer de entrada com enquadramento parcial, fila de saida com deslocamento da mensagem atual, instante do ultimo progresso (timeout de ociosidade), estado de autenticacao (anonimo/autenticado), estado da maquina do protocolo e o orcamento de memoria que aquela conexao pode ocupar.

**Autoverificacao:** `ECHO ola` responde `ola`, linha sem `\n` acima de 256 bytes encerra a conexao, dois clientes simultaneos nao se bloqueiam e a fila de saida drena mensagem a mensagem.
