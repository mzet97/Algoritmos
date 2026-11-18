---
layout: default
---

# Aula 28 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../28.md#19-exercicios-obrigatorios).

1. Compacto contra largo, com a mesma soma:

```cpp
#include <chrono>
#include <cstddef>
#include <iostream>
#include <vector>

struct Largo {
    double valor;
    char enchimento[56]; // um elemento por linha de 64 bytes
};

static double mede(const char* nome, auto&& fn)
{
    auto inicio = std::chrono::steady_clock::now();
    double resultado = fn();
    auto fim = std::chrono::steady_clock::now();
    double us = std::chrono::duration<double, std::micro>(fim - inicio).count();
    std::cout << nome << "_us=" << us << " soma=" << resultado << '\n';
    return us;
}

int main()
{
    const std::size_t N = 200000;
    std::vector<double> compacto(N, 1.0);
    std::vector<Largo> largo(N);
    for (auto& e : largo) {
        e.valor = 1.0;
    }

    mede("compact", [&] {
        double s = 0.0;
        for (double x : compacto) s += x;
        return s;
    });
    mede("wide", [&] {
        double s = 0.0;
        for (const auto& e : largo) s += e.valor;
        return s;
    });
    std::cout << "sizeof Largo: " << sizeof(Largo) << '\n';
    return 0;
}
```

As somas sao identicas (`200000`); os tempos impressos sao desta maquina e desta execucao — rode 3 vezes e compare a ordem de grandeza, nao o digito (protocolo da [aula 28a](../../c/exercicios/28-solucoes.md)). O largo traz uma linha de cache inteira por elemento util; o compacto, 8 doubles por linha.

2. False sharing: escritas de variaveis **diferentes** na mesma linha por nucleos diferentes invalidam a linha um do outro a cada escrita. Sem variavel compartilhada, sem data race — e lento do mesmo jeito, como a [aula 28 de C](../../c/exercicios/28-solucoes.md) mede.

3. `alignas(64)` ajuda exatamente ai: contadores por thread num array comum ganham uma linha cada, e a coerencia para de disputar. Fora desse desenho (um contador so, acesso de uma thread), alinhar e desperdicio de memoria.

4. SoA (um array por campo) vence quando o laco quente le um ou dois campos de muitos registros: linhas so com bytes uteis e SIMD direto. AoS vence quando o acesso usa o registro inteiro por vez ou segue a identidade do objeto. A ponte [30b](30b_pistas.md) mede o parente SIMD dessa conversa.

5. Gargalo em layout/cache costuma mostrar: instrucoes por ciclo baixas com CPU "ocupada", `cache-misses` altos no `perf stat` e escalonamento que para de melhorar ao adicionar threads em trabalho independente (coerencia). Algoritmo puro ruim, em contraste, escala com nucleos e tem IPC saudavel.

**Autoverificacao:** somas iguais, `sizeof Largo` 64 e os tempos declarados como desta maquina com repeticoes.
