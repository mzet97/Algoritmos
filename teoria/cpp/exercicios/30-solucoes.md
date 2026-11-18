---
layout: default
---

# Aula 30 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../30.md#21-exercicios-obrigatorios).

1 e 2. Os tres saxpy e o relatorio do compilador:

```cpp
#include <cstddef>
#include <iostream>
#include <vector>

static void saxpy_scalar(float a, const float* x, float* y, std::size_t n)
{
    for (std::size_t i = 0; i < n; i++) {
        y[i] = a * x[i] + y[i];
    }
}

static void saxpy_branchy(float a, const float* x, float* y, std::size_t n)
{
    for (std::size_t i = 0; i < n; i++) {
        if (x[i] > 0.0f) {
            y[i] = a * x[i] + y[i];
        }
    }
}

static void saxpy_restrict(float a, const float* __restrict x,
                           float* __restrict y, std::size_t n)
{
    for (std::size_t i = 0; i < n; i++) {
        y[i] = a * x[i] + y[i];
    }
}

int main()
{
    const std::size_t n = 1000000;
    std::vector<float> x(n), y(n, 1.0f);
    for (std::size_t i = 0; i < n; i++) {
        x[i] = static_cast<float>(static_cast<int>(i % 13) - 6);
    }
    saxpy_scalar(2.0f, x.data(), y.data(), n);
    saxpy_branchy(2.0f, x.data(), y.data(), n);
    saxpy_restrict(2.0f, x.data(), y.data(), n);
    std::cout << "y[6]=" << y[6] << " y[7]=" << y[7] << '\n';
    return 0;
}
```

```bash
g++ -std=c++20 -O3 -march=native -fopt-info-vec -c saxpy.cpp -o /dev/null
```

Nesta rodada, o g++ 16 relatou o laco escalar vetorizado (`loop vectorized using 32 byte vectors`); a forma com desvio por elemento nao recebeu o mesmo relatorio — vetor com mascara tem custo e o compilador escolhe. O relatorio da sua maquina, com versao e flags anotadas, e a evidencia que vale (protocolo da [aula 28a](../../c/exercicios/28-solucoes.md)); `-march=native` amarra o binario a esta CPU.

3. Sem `__restrict`, `x` e `y` poderiam apontar para regioes sobrepostas, e escrever `y[i]` mudaria um `x[j]` futuro — o compilador ou vetoriza com checagens de versao ou nao vetoriza. A promessa remove o caso; quebra-la e comportamento indefinido.

4. SoA ajudaria mais que AoS quando o kernel le um campo so de muitos registros (posicoes de particulas, por exemplo): elementos consecutivos do campo alimentam o registrador vetorial sem gathers.

5. Intrinsicos se justificam com medicao neste kernel quente, alvo conhecido e ganho que pague portabilidade e legibilidade — a ponte [30b](30b_pistas.md) executa esse degrau com oraculo em dobro.

**Autoverificacao:** relatorio colado com flags/versao, `y[6]`/`y[7]` coerentes com o sinal de `x` e o preco do `-march=native` escrito.
