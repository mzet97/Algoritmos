// Servidor TCP de eco com I/O nao bloqueante, backpressure e timeouts.
// Linux, loopback apenas. Uso: ./srv23a [porta] [ocioso_seg]
//   porta padrao 9091; ocioso padrao 10 s (0 desliga o timeout, so p/ teste).
#include <arpa/inet.h>
#include <cerrno>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <poll.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

namespace {

constexpr int MAX_CLIENTES = 32;
constexpr std::size_t LINHA_MAX = 4096;
constexpr std::size_t ENTRADA_MAX = 8192;
constexpr std::size_t SAIDA_MAX = 65536;

using Relogio = std::chrono::steady_clock;

struct Conexao {
    int fd = -1;
    std::string entrada;
    std::string saida;
    std::size_t enviada = 0; // bytes da saida ja escritos
    Relogio::time_point ultima_atividade = Relogio::now();
    bool fechando = false;
    std::uint64_t id = 0;
};

bool nao_bloqueante(int fd) {
    int f = fcntl(fd, F_GETFL, 0);
    if (f < 0) {
        return false;
    }
    return fcntl(fd, F_SETFL, f | O_NONBLOCK) == 0;
}

// Enfileira resposta; false = sem espaco (backpressure: fechar).
bool enfileirar(Conexao &c, const char *dados, std::size_t n) {
    if (c.saida.size() - c.enviada + n > SAIDA_MAX) {
        return false;
    }
    c.saida.append(dados, n);
    c.ultima_atividade = Relogio::now();
    return true;
}

// Processa linhas completas; false = protocolo violado (fechar).
bool consumir_linhas(Conexao &c) {
    for (;;) {
        std::size_t fim = c.entrada.find('\n');
        if (fim == std::string::npos) {
            return true; // parcial: o teto de ENTRADA_MAX decide em ler_cliente
        }
        std::string linha = c.entrada.substr(0, fim + 1);
        c.entrada.erase(0, fim + 1);
        if (linha.size() > LINHA_MAX) {
            // Linha completa mas acima do teto: derruba sem responder.
            // Responder aqui criaria oraculo para sondagem de protocolo.
            // O fechamento pode chegar como RST se houver dados nao lidos:
            // esperado e aceito (ver teste), pois a alternativa seria ler
            // lixo do atacante para merecer um FIN.
            return false;
        }
        const char *r = nullptr;
        std::size_t rn = 0;
        char eco[LINHA_MAX + 8];
        if (linha == "PING\n") {
            r = "PONG\n";
            rn = 5;
        } else if (linha == "QUIT\n") {
            if (!enfileirar(c, "BYE\n", 4)) {
                return false;
            }
            c.fechando = true;
            return true; // ignora o resto: conexao ja esta de saida
        } else if (linha.rfind("ECHO ", 0) == 0) {
            // Ecoa o resto da linha como veio (sem NUL de C-string).
            std::size_t n = linha.size() - 5;
            if (n + 1 > sizeof eco) {
                return false;
            }
            std::memcpy(eco, linha.data() + 5, n);
            eco[n] = '\0';
            r = eco;
            rn = n;
            // 'eco' morre no fim do bloco: copiar antes de enfileirar.
            std::string tmp(r, rn);
            if (!enfileirar(c, tmp.data(), tmp.size())) {
                return false;
            }
            continue;
        } else {
            r = "ERR comando desconhecido\n";
            rn = 23;
        }
        if (!enfileirar(c, r, rn)) {
            return false;
        }
    }
}

bool ler_cliente(Conexao &c) {
    char buf[1024];
    for (;;) {
        ssize_t n = recv(c.fd, buf, sizeof buf, 0);
        if (n > 0) {
            if (c.entrada.size() + (std::size_t)n > ENTRADA_MAX) {
                // Sem terminador em 8 KiB: derruba na hora. Drenar antes
                // de fechar exigiria ler lixo do atacante para merecer um
                // FIN — e travaria se ele parasse de enviar no meio. O
                // fechamento pode chegar como RST quando ainda houver bytes
                // nao lidos no kernel: esperado e aceito (ver teste).
                return false;
            }
            c.entrada.append(buf, (std::size_t)n);
            c.ultima_atividade = Relogio::now();
            if (!consumir_linhas(c)) {
                return false;
            }
            continue;
        }
        if (n == 0) {
            return false; // EOF: fecha limpo no chamador
        }
        if (errno == EINTR) {
            continue;
        }
        return errno == EAGAIN || errno == EWOULDBLOCK; // true = tente depois
    }
}

bool escrever_cliente(Conexao &c) {
    while (c.enviada < c.saida.size()) {
        ssize_t n = send(c.fd, c.saida.data() + c.enviada,
                         c.saida.size() - c.enviada, MSG_NOSIGNAL);
        if (n > 0) {
            c.enviada += (std::size_t)n;
            c.ultima_atividade = Relogio::now();
            continue;
        }
        if (n < 0 && (errno == EINTR)) {
            continue;
        }
        // EAGAIN: o kernel pede pausa; erro real ou 0: fechar.
        return n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK);
    }
    if (c.enviada == c.saida.size()) {
        c.saida.clear();
        c.enviada = 0;
    }
    return true;
}

} // namespace

int main(int argc, char **argv) {
    int porta = 9091;
    long ocioso = 10;
    if (argc > 1) {
        porta = std::atoi(argv[1]);
    }
    if (argc > 2) {
        ocioso = std::atol(argv[2]);
    }
    if (porta <= 0 || porta > 65535 || ocioso < 0) {
        std::fprintf(stderr, "uso: %s [porta] [ocioso_seg]\n", argv[0]);
        return 2;
    }

    int ouvinte = socket(AF_INET, SOCK_STREAM, 0);
    if (ouvinte < 0) {
        return 1;
    }
    int opt = 1;
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons((std::uint16_t)porta);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (setsockopt(ouvinte, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt) < 0 ||
        !nao_bloqueante(ouvinte) ||
        bind(ouvinte, (sockaddr *)&addr, sizeof addr) < 0 || listen(ouvinte, 16) < 0) {
        std::perror("listen");
        return 1;
    }

    std::vector<Conexao> clientes;
    std::uint64_t proximo_id = 1;
    std::printf("127.0.0.1:%d ocioso=%lds\n", porta, ocioso);
    std::fflush(stdout);

    for (;;) {
        std::vector<pollfd> fds;
        fds.push_back({ouvinte, POLLIN, 0});
        for (auto &c : clientes) {
            if (c.fd < 0) {
                continue;
            }
            short ev = 0;
            if (!c.fechando) {
                ev |= POLLIN;
            }
            if (c.enviada < c.saida.size()) {
                ev |= POLLOUT;
            }
            fds.push_back({c.fd, ev, 0});
        }
        int prontos = poll(fds.data(), fds.size(), 100);
        if (prontos < 0) {
            if (errno == EINTR) {
                continue;
            }
            std::perror("poll");
            return 1;
        }
        auto agora = Relogio::now();

        if (fds[0].revents & POLLIN) {
            for (;;) {
                int fd = accept(ouvinte, nullptr, nullptr);
                if (fd < 0) {
                    break; // EAGAIN: sem mais pendentes
                }
                if (!nao_bloqueante(fd)) {
                    close(fd);
                    continue;
                }
                if ((int)clientes.size() >= MAX_CLIENTES) {
                    close(fd); // teto global: recusa, nao enfileira
                    continue;
                }
                Conexao c;
                c.fd = fd;
                c.id = proximo_id++;
                clientes.push_back(std::move(c));
            }
        }

        for (auto it = clientes.begin(); it != clientes.end();) {
            bool viva = true;
            auto festa = [&](int fd) {
                for (auto &p : fds) {
                    if (p.fd == fd) {
                        return p.revents;
                    }
                }
                return (short)0;
            };
            short ev = festa(it->fd);
            if (ev & (POLLERR | POLLNVAL)) {
                viva = false;
            } else {
                if ((ev & POLLIN) && !it->fechando && !ler_cliente(*it)) {
                    viva = false;
                }
                if (viva && (ev & POLLOUT) && !escrever_cliente(*it)) {
                    viva = false;
                }
                if (viva && it->fechando && it->enviada == it->saida.size()) {
                    viva = false; // QUIT respondido e drenado
                }
                if (viva && ocioso > 0 &&
                    agora - it->ultima_atividade > std::chrono::seconds(ocioso)) {
                    viva = false; // ocioso demais: libera o slot
                }
            }
            if (!viva) {
                close(it->fd);
                it = clientes.erase(it);
            } else {
                ++it;
            }
        }
    }
}
