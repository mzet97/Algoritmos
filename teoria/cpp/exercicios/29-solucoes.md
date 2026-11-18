---
layout: default
---

# Aula 29 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../29.md#21-exercicios-obrigatorios). Programa de apoio e roteiro executado:

```cpp
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <vector>

static double sum_linear(const std::vector<double>& v)
{
    double s = 0.0;
    for (double x : v) {
        s += x;
    }
    return s;
}

static double sum_stride(const std::vector<double>& v, std::size_t passo)
{
    double s = 0.0;
    for (std::size_t i = 0; i < v.size(); i += passo) {
        s += v[i];
    }
    return s;
}

int main(int argc, char* argv[])
{
    std::size_t n = argc > 1 ? static_cast<std::size_t>(std::atol(argv[1])) : 8000000;
    std::vector<double> v(n, 1.0);
    std::cout << "linear: " << static_cast<long long>(sum_linear(v)) << '\n';
    std::cout << "stride8: " << static_cast<long long>(sum_stride(v, 8)) << '\n';
    return 0;
}
```

```bash
g++ -std=c++20 -O2 -g -o perfil perfil.cpp
perf stat -e cycles,instructions,cache-misses ./perfil
perf record ./perfil && perf report --stdio | head
```

Confira na sua execucao (valores sao desta maquina, nao constantes): somas `8000000` e `1000000`, ciclos por elemento muito maiores no stride e a funcao quente onde as amostras estao. A leitura completa dos contadores segue a [aula 29 de C](../../c/exercicios/29-solucoes.md), cujo programa e o gemeo deste.

2. Iteracao nao e unidade de custo: passo 8 toca uma linha nova por elemento (64 bytes para usar 8); o laco linear usa cada linha 8 vezes. Menos passos caros pode perder para mais passos baratos.

4. Callgrind (valgrind) conta instrucoes executadas e monta a arvore de chamadas — otimo para a estrutura de custo e deterministico, mas cego a cache e relogio. `perf` amostra o hardware real. Um diz como o custo se organiza; outro, onde ele doi.

5 e 6. `branch-misses` alto faria sentido com desvio dependente de dados imprevisiveis dentro do laco (somar condicionalmente valores aleatorios); nestes lacos os desvios sao de controle e previsiveis. `cache-misses` alto no stride e geometria: cada acesso estreia uma linha, usa 8 bytes e o prefetch nao encontra o padrao simples do laco vizinho.

**Autoverificacao:** somas conferidas antes dos contadores, funcao quente lida do report e uma frase ligando passo de 64 bytes, linha e falta.
