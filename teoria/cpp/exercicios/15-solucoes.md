---
layout: default
---

# Aula 15 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../15.md#16-exercicios-obrigatorios).

1 e 2. Percurso e reserva, medidos no mesmo programa:

```cpp
#include <chrono>
#include <cstddef>
#include <iostream>
#include <list>
#include <vector>

int main()
{
    const std::size_t N = 200000;
    std::vector<int> v;
    std::list<int> l;
    for (std::size_t i = 0; i < N; i++) {
        v.push_back(static_cast<int>(i));
        l.push_back(static_cast<int>(i));
    }

    auto mede = [](const char* nome, auto&& fn) {
        auto inicio = std::chrono::steady_clock::now();
        long long resultado = fn();
        auto fim = std::chrono::steady_clock::now();
        double ms = std::chrono::duration<double, std::milli>(fim - inicio).count();
        std::cout << nome << ": soma=" << resultado << " tempo_ms=" << ms << '\n';
        return ms;
    };

    mede("vector", [&] {
        long long s = 0;
        for (int x : v) s += x;
        return s;
    });
    mede("list", [&] {
        long long s = 0;
        for (int x : l) s += x;
        return s;
    });

    std::vector<int> sem_reserva;
    for (std::size_t i = 0; i < N; i++) sem_reserva.push_back(static_cast<int>(i));
    std::vector<int> com_reserva;
    com_reserva.reserve(N);
    for (std::size_t i = 0; i < N; i++) com_reserva.push_back(static_cast<int>(i));
    std::cout << "capacidades: " << sem_reserva.capacity() << ' ' << com_reserva.capacity() << '\n';
    return 0;
}
```

As somas sao identicas; os tempos desta maquina sao impressos pelo programa (rode 3 vezes e compare medianas, protocolo da [aula 28a de C](../../c/exercicios/28-solucoes.md)). O `vector` entrega varios elementos por linha de cache e prefetch sequencial; a `list` paga um salto de memoria por no.

3. Contiguo significa enderecos consecutivos: uma linha de 64 bytes traz 16 `int` do vetor, e o hardware preve os proximos. Na lista, cada linha traz o no e seus ponteiros — poucos bytes uteis e nenhum padrao prefetchavel.

4. Tres derrotas classicas da estrutura "elegante": alocacao por elemento (custo e fragmentacao), iteracao por perseguicao de ponteiro (cache) e ausencia de acesso aleatorio/contiguidade (algoritmos e SIMD ficam de fora). Insercao O(1) no meio so vence se o trabalho dominante for esse **e** o iterador ja estiver na mao.

5. Otimizar sem perfil e chutar onde o tempo mora: o perfil diz a funcao quente e a fracao dela no total (lei de Amdahl limita o ganho possivel). Sem isso, voce pode dobrar a velocidade de 3% do programa. A [aula 29 de C](../../c/exercicios/29-solucoes.md) mostra o roteiro com `perf`.

**Autoverificacao:** somas iguais, tempos declarados como desta maquina com repeticoes e um motivo de derrota da lista que cite bytes uteis por linha.
