---
layout: default
---

# Aula 22 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../22.md#24-exercicios-obrigatorios).

1. Gerador minimo com ordem de impressao determinista:

```cpp
#include <coroutine>
#include <iostream>

struct Gerador {
    struct promise_type {
        int valor_atual = 0;
        Gerador get_return_object()
        {
            return Gerador{Handle::from_promise(*this)};
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(int v)
        {
            valor_atual = v;
            return {};
        }
        void return_void() {}
        void unhandled_exception() {}
    };

    using Handle = std::coroutine_handle<promise_type>;

    explicit Gerador(Handle handle) : h(handle) {}
    ~Gerador()
    {
        if (h) {
            h.destroy();
        }
    }
    Gerador(const Gerador&) = delete;
    Gerador& operator=(const Gerador&) = delete;

    bool proximo()
    {
        if (!h || h.done()) {
            return false;
        }
        h.resume();
        return !h.done();
    }
    int valor() const { return h.promise().valor_atual; }

    Handle h;
};

Gerador conta()
{
    std::cout << "2: corpo comecou\n";
    co_yield 10;
    std::cout << "4: retomou apos o primeiro yield\n";
    co_yield 20;
}

int main()
{
    std::cout << "1: antes de criar\n";
    auto g = conta();
    std::cout << "3: criada; corpo nao rodou (initial_suspend)\n";
    while (g.proximo()) {
        std::cout << "valor: " << g.valor() << '\n';
    }
    std::cout << "5: fim\n";
    return 0;
}
```

Saida exata: `1`, `3`, `2`, `valor: 10`, `4`, `valor: 20`, `5`. Os pontos fixos: (a) com `initial_suspend` em `suspend_always`, o corpo **nao** comeca na criacao — comeca na primeira retomada (`proximo`); (b) cada `co_yield` devolve o controle ao chamador com um valor; (c) o frame so e destruido porque `~Gerador` chama `h.destroy()` — sem dono, ele vazaria parado no ultimo ponto de suspensao.

2. O **coroutine frame** e o registro de ativacao que vive fora da pilha comum: guarda parametros, variaveis locais que sobrevivem a suspensoes, o estado (em que ponto retomar) e a promessa. Ele existe porque a coroutine pode suspender, a thread pode fazer outra coisa e a retomada pode acontecer depois em outra pilha — nada disso cabe num registro de ativacao comum.

3. `future.get()` **bloqueia a thread** ate o valor existir. `co_await` numa coroutine **suspende so aquela coroutine** e devolve a thread ao escalonador/loop: o custo da espera sai da thread e vai para o frame parado.

4. O codigo fonte le-se de cima para baixo, mas cada suspensao permite que outras coroutines (ou eventos) rodem entre duas linhas suas. Raciocinar como se fosse execucao atomica entre `co_await`s vizinhos sem justificar e a versao coroutine do data race logico.

5. Antes de producao, pergunte: quem e o dono do frame e quem o destroi se ninguem retomar? As referencias e ponteiros capturados continuam vivos em **todas** as retomadas possiveis? Como se cancela — e quem observa o cancelamento? Se qualquer resposta for "ninguem sabe", a coroutine ainda nao esta pronta.

**Autoverificacao:** ordem anotada a partir de uma execucao sua (nao desta pagina), o frame descrito sem mencionar pilha e as tres perguntas de ownership respondidas para um exemplo seu.
