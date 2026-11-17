---
layout: default
---

# Aula 14 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../14.md#15-exercicios-obrigatorios).

```cpp
#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>

int main()
{
    std::thread t1([] { std::cout << "mensagem da thread 1\n"; });
    std::thread t2([] { std::cout << "mensagem da thread 2\n"; });
    t1.join();
    t2.join();

    const int REPETICOES = 100000;
    int contador_mutex = 0;
    std::mutex m;
    auto soma_mutex = [&] {
        for (int i = 0; i < REPETICOES; i++) {
            std::lock_guard<std::mutex> trava(m);
            contador_mutex++;
        }
    };
    std::atomic<int> contador_atomico{0};
    auto soma_atomico = [&] {
        for (int i = 0; i < REPETICOES; i++) {
            contador_atomico.fetch_add(1, std::memory_order_relaxed);
        }
    };

    std::thread a(soma_mutex), b(soma_mutex);
    a.join();
    b.join();
    std::thread c(soma_atomico), d(soma_atomico);
    c.join();
    d.join();

    std::cout << "mutex: " << contador_mutex << '\n';     // 200000
    std::cout << "atomico: " << contador_atomico << '\n'; // 200000
    return 0;
}
```

A ordem das duas primeiras mensagens pode inverter entre execucoes; os contadores, nao — 200000 nas duas versoes, em qualquer escalonamento.

4. Em C++, data race (acessos concorrentes ao mesmo objeto, pelo menos uma escrita, sem sincronizacao) e **comportamento indefinido**, nao "valor imprevisivel": o compilador otimiza supondo que ele nao existe. E por isso que remover o mutex "e funcionou" nao prova nada, como na [aula 17a](17a_pistas.md) e na [aula 14a de C](../../c/exercicios/14-solucoes.md).

5. Mutex serializa uma **regiao**: protege invariantes que envolvem varias variaveis e operacoes compostas, ao custo de bloquear. Atomico oferece operacoes indivisiveis sobre **uma** variavel (incrementar, trocar, comparar-e-trocar) sem bloquear. Contador isolado pede atomico; transferir valor entre duas contas pede mutex (ou uma transacao equivalente), porque o invariante mora no par.

**Autoverificacao:** 200000 exatos nas duas versoes em execucoes repetidas, toda thread com `join` e um exemplo seu de invariante que atomico sozinho nao protege.
