---
layout: default
---

# Aula 20 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../20.md#20-exercicios-obrigatorios).

1. No **reactor**, o loop pergunta "quem esta pronto?" (`poll`/`epoll`) e chama o seu codigo para fazer a operacao — voce executa o `read`. No **proactor**, voce **posta** a operacao ("leia ate 4 KB neste buffer") e o sistema chama seu codigo quando ela terminou, com o resultado pronto (modelo do `io_uring`/IOCP). Reactor e o padrao das aulas [20](../../c/exercicios/20-solucoes.md) e 21 de C; proactor troca prontidao por conclusao.

2. Handler de timer como cidadao do loop — mini-reactor executavel (timer a cada 100 ms, 3 disparos, sem nenhuma conexao):

```cpp
#include <chrono>
#include <functional>
#include <iostream>
#include <thread>
#include <vector>

struct Timer {
    std::chrono::steady_clock::time_point prazo;
    std::function<void()> callback;
};

int main()
{
    using namespace std::chrono;
    std::vector<Timer> timers;
    int disparos = 0;
    timers.push_back({steady_clock::now() + milliseconds(100),
                      [&] {
                          disparos++;
                          std::cout << "timer disparou " << disparos << '\n';
                      }});

    while (disparos < 3) {
        // Num reactor real, este sono e o timeout do poll/epoll:
        // acorda no menor prazo pendente ou num evento de rede.
        std::this_thread::sleep_until(timers.front().prazo);
        auto callback = timers.front().callback;
        timers.front().prazo = steady_clock::now() + milliseconds(100);
        callback();
    }
    std::cout << "total: " << disparos << '\n';
    return 0;
}
```

Num servidor, o `sleep_until` vira o timeout calculado do `poll`: o loop acorda pelo que vier primeiro, evento de rede ou prazo — e handlers de timer cuidam de timeouts de conexao, retries e heartbeats sem thread dedicada.

3. Thread por conexao e simples e inadequado quando as conexoes sao milhares e majoritariamente ociosas: paga-se pilha e escalonamento por conexao parada, e o teto chega por memoria antes de chegar por trabalho util (comparacao completa na [aula 21 de C](../../c/exercicios/21-solucoes.md)).

4. Estado por conexao num servidor assincrono real: fd, buffer de entrada com o enquadramento parcial, buffer de saida com o deslocamento ja enviado, estado da maquina do protocolo, instante do ultimo progresso (para timeout) e o dono desses buffers. Esquecer o deslocamento de saida e o classico: `send` parcial sem memoria do que falta corrompe a resposta.

5. Em callbacks e continuacoes, o perigo e tempo de vida: a continuacao roda **depois**, possivelmente quando o objeto que ela capturou por referencia ja morreu (a conexao fechou, o request acabou). Por isso codigo assincrono prefere capturar por valor, `shared_ptr` com papel claro ou tokens de cancelamento — o problema nao e a sintaxe da lambda, e quem garante que o capturado ainda existe.

**Autoverificacao:** `timer disparou 1..3` e `total: 3`, a diferenca reactor/proactor dita com "quem executa o read" e um estado por conexao que inclua deslocamento de saida.
