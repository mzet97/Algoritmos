---
layout: default
---

# Aula 18 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../18.md#14-exercicios-obrigatorios).

1 e 2. Wrapper de fd com `release()`, copia deletada e movimento:

```cpp
#include <iostream>
#include <utility>
#include <unistd.h>

class SocketFd {
public:
    explicit SocketFd(int fd = -1) : fd_(fd) {}
    ~SocketFd() { reset(); }

    SocketFd(const SocketFd&) = delete;
    SocketFd& operator=(const SocketFd&) = delete;

    SocketFd(SocketFd&& outro) noexcept : fd_(std::exchange(outro.fd_, -1)) {}
    SocketFd& operator=(SocketFd&& outro) noexcept
    {
        if (this != &outro) {
            reset();
            fd_ = std::exchange(outro.fd_, -1);
        }
        return *this;
    }

    int get() const { return fd_; }

    // Entrega o fd SEM fechar; o wrapper fica vazio e quem recebe e o dono.
    int release()
    {
        return std::exchange(fd_, -1);
    }

    void reset(int novo = -1)
    {
        if (fd_ >= 0) {
            close(fd_);
        }
        fd_ = novo;
    }

private:
    int fd_;
};

int main()
{
    int par[2];
    if (pipe(par) != 0) {
        return 1;
    }
    SocketFd leitura{par[0]};
    SocketFd escrita{par[1]};

    const char msg[] = "oi";
    ssize_t enviado = write(escrita.get(), msg, sizeof msg);
    char buf[8] = {};
    ssize_t lidos = read(leitura.get(), buf, sizeof buf);
    std::cout << "enviado=" << enviado << " lidos=" << lidos << " dados=" << buf << '\n';

    int entregue = escrita.release();
    std::cout << "wrapper apos release: " << escrita.get() << '\n'; // -1
    close(entregue); // dono agora e este codigo
    return 0;
}
```

2. Copiar o wrapper duplicaria a posse do mesmo fd: os dois destrutores chamariam `close` no mesmo numero — e como numeros de fd sao reutilizados, o segundo `close` pode encerrar um arquivo aberto depois, sem relacao alguma. Por isso dono de fd se move, nao se copia (regra dos 5 da [aula 05](05-solucoes.md)).

3. `send` e `recv` tem tres desfechos com significados distintos: positivo parcial (faltou parte — continue), zero em `recv` (o outro lado fechou em ordem) e -1 com `errno` (`EAGAIN` = tente depois; `EINTR` = tente de novo; outros = erro real). Tratar qualquer retorno como "tudo certo" corrompe protocolo silenciosamente, como o servidor da [aula 23a](../23a.md) endurece.

4. Como TCP e pipe sao fluxo de bytes, uma leitura pode trazer meia mensagem ou duas mensagens coladas; framing (delimitador ou comprimento, com teto) e o que transforma fluxo em mensagens — implementado nas aulas [15](../../c/exercicios/15-solucoes.md) e [20](../../c/exercicios/20-solucoes.md) de C.

5. Neste arquivo: C++ e a classe, RAII, `std::exchange` e a delecao de copia. API nativa (POSIX) sao `pipe`, `read`, `write`, `close` e o tipo `ssize_t`. O wrapper existe exatamente para costurar os dois mundos: sintaxe e garantia de C++ sobre recursos do sistema operacional.

**Autoverificacao:** `get()` vale -1 apos `release()` sem o fd ter sido fechado pelo wrapper, o programa troca a mensagem pelo pipe e compilar uma copia do wrapper falha.
