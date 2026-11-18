---
layout: default
---

# Aula 27 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../27.md#21-exercicios-obrigatorios).

1, 2 e 3. Contexto propagado, span RAII e metricas com histograma:

```cpp
#include <array>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <string>

struct Contexto {
    long request_id;
};

class ScopedSpan {
public:
    ScopedSpan(const Contexto& ctx, std::string nome)
        : ctx_(ctx), nome_(std::move(nome)),
          inicio_(std::chrono::steady_clock::now())
    {
        std::cout << "[req " << ctx_.request_id << "] inicio " << nome_ << '\n';
    }
    ~ScopedSpan()
    {
        auto fim = std::chrono::steady_clock::now();
        double ms = std::chrono::duration<double, std::milli>(fim - inicio_).count();
        std::cout << "[req " << ctx_.request_id << "] fim " << nome_
                  << " duracao_ms=" << ms << '\n';
    }

private:
    Contexto ctx_;
    std::string nome_;
    std::chrono::steady_clock::time_point inicio_;
};

struct Metricas {
    long conexoes_aceitas = 0;
    long erros = 0;
    // buckets: <1ms, <10ms, <100ms, >=100ms
    std::array<long, 4> histograma{};

    void registra_latencia(double ms)
    {
        if (ms < 1.0) histograma[0]++;
        else if (ms < 10.0) histograma[1]++;
        else if (ms < 100.0) histograma[2]++;
        else histograma[3]++;
    }
};

void atende(const Contexto& ctx, Metricas& m)
{
    ScopedSpan span{ctx, "atende"};
    m.conexoes_aceitas++;
    std::cout << "[req " << ctx.request_id << "] processando\n";
}

int main()
{
    Metricas m;
    atende({101}, m);
    atende({102}, m);
    m.registra_latencia(0.5);
    m.registra_latencia(3.0);
    m.registra_latencia(3.5);
    m.registra_latencia(250.0);
    std::cout << "aceitas=" << m.conexoes_aceitas << " erros=" << m.erros << '\n';
    std::cout << "histograma:";
    for (long b : m.histograma) {
        std::cout << ' ' << b;
    }
    std::cout << '\n';
    return 0;
}
```

O `request_id` aparece na abertura, no corpo e no fechamento de cada request: e isso que permite filtrar o log inteiro de uma requisicao num servidor concorrente, onde linhas de requests diferentes se interleiam. Sem ele, observabilidade em concorrencia e arqueologia.

2. O histograma troca a lista de latencias por contadores por faixa: memoria constante, agregavel entre processos e suficiente para estimar percentis ("95% abaixo de 100 ms" le-se da acumulada). A media sozinha esconderia exatamente a cauda que o histograma expoe.

4. Minimo de um servidor TCP pequeno: conexoes ativas (gauge), total aceitas e total encerradas por motivo, requisicoes por comando, erros por classe, latencia em histograma e bytes lidos/escritos. Cada metrica responde uma pergunta operacional nomeada; metrica sem pergunta e ruido caro.

5. Logs sao insuficientes quando a pergunta e quantitativa e continua — "a latencia p99 piorou depois do deploy?" — porque exigiria processar milhoes de linhas para agregar o que um contador/histograma ja mantem pronto. Tracing entra quando a pergunta e o caminho de **uma** requisicao por varios servicos.

**Autoverificacao:** `histograma: 1 2 0 1`, toda linha do request com seu id e span fechando mesmo num caminho com retorno antecipado (teste o seu).
