---
layout: default
---

# Aula 21 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../21.md#21-exercicios-obrigatorios).

1, 2 e 3. Promise/future, pool com tres tarefas e excecao propagada:

```cpp
#include <future>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

int main()
{
    std::promise<int> promessa;
    std::future<int> futuro = promessa.get_future();
    std::thread entregadora([](std::promise<int> p) {
        p.set_value(42);
    }, std::move(promessa));
    std::cout << "promise/future: " << futuro.get() << '\n';
    entregadora.join();

    std::vector<std::future<int>> resultados;
    for (int i = 1; i <= 3; i++) {
        resultados.push_back(std::async(std::launch::async, [i] {
            if (i == 3) {
                throw std::runtime_error("tarefa 3 falhou");
            }
            return i * 10;
        }));
    }
    for (auto& f : resultados) {
        try {
            std::cout << "tarefa: " << f.get() << '\n';
        } catch (const std::exception& e) {
            std::cout << "excecao propagada: " << e.what() << '\n';
        }
    }
    return 0;
}
```

Saida: `promise/future: 42`, `tarefa: 10`, `tarefa: 20`, `excecao propagada: tarefa 3 falhou`. Nada de codigo de erro manual: a excecao viaja no pacote e renasce no `get()` de quem esperou.

4. `future.get()` bloqueia a thread chamadora ate a entrega. Dentro da thread de um event loop, bloquear ali congela **todas** as conexoes e timers que aquele loop atende — o desenho da [aula 20](20-solucoes.md) morre por um `get()`. Em loop, ou a tarefa e tao rapida que nao devia ser assincrona, ou o resultado volta por callback/fila vigiada, nunca por espera.

5. Pool de tarefas reparte **trabalho de CPU** entre threads: cada tarefa roda inteira numa thread ate acabar. Multiplexacao de I/O reparte **espera** entre descritores numa thread: a thread dorme no `poll` e acorda quem esta pronto. Servidores reais combinam os dois, como a [aula 21 de C](../../c/exercicios/21-solucoes.md) desenha — confundir os papeis gera thread por conexao ociosa ou loop bloqueado em computacao.

**Autoverificacao:** as tres tarefas colhidas (a terceira como excecao), nenhum `get()` dentro de loop de eventos no seu desenho e a frase "pool reparte CPU, multiplexacao reparte espera" com suas palavras.
