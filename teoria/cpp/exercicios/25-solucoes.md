---
layout: default
---

# Aula 25 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../25.md#22-exercicios-obrigatorios).

1. A mesma API nas duas formas:

```cpp
#include <functional>
#include <iostream>
#include <string>
#include <vector>

template <typename F>
void aplica_template(const std::vector<int>& v, F f)
{
    for (int x : v) {
        std::cout << f(x) << ' ';
    }
    std::cout << '\n';
}

void aplica_function(const std::vector<int>& v, const std::function<int(int)>& f)
{
    for (int x : v) {
        std::cout << f(x) << ' ';
    }
    std::cout << '\n';
}

int main()
{
    std::vector<int> v{1, 2, 3};
    int soma = 0;
    auto acumula = [&soma](int x) {
        soma += x;
        return soma;
    };
    aplica_template(v, acumula);          // tipo da lambda entra na assinatura
    std::cout << "soma via template: " << soma << '\n';

    soma = 0;
    aplica_function(v, acumula);          // a mesma lambda, tipo apagado
    std::cout << "soma via function: " << soma << '\n';

    std::function<int(int)> guardado = [](int x) { return x * 2; };
    std::cout << "guardado: " << guardado(21) << '\n';
    return 0;
}
```

Ergonomia: o template aceita qualquer chamavel sem declarar tipo e pode embutir a chamada, mas a definicao precisa estar no header e cada chamavel gera uma instancia. `std::function` fixa um tipo so para todos os chamaveis — da para guardar em containers e membros — ao custo de chamada indireta e possivel alocacao.

2. **Type erasure** e esse truque: por fora, uma interface unica e conhecida (`operator()` com assinatura fixa); por dentro, um ponteiro para o objeto concreto de tipo qualquer, com as operacoes (chamar, copiar, destruir) despachadas por uma tabela interna. `std::function`, `std::any` e o handle opaco da [aula 17](17-solucoes.md) sao o mesmo desenho em escalas diferentes.

3. Para evitar alocacao sempre, implementacoes guardam um buffer interno pequeno (SBO); chamaveis com capturas grandes nao cabem e vao para o heap. O tamanho do buffer nao e garantido pelo padrao — outro motivo para medir em vez de presumir.

4. Ponteiro para funcao vence onde a fronteira e C (a [aula 17](17-solucoes.md) exporta simbolos C), onde excecoes/alocacao sao proibidas ou onde o chamavel nao tem estado algum e o custo precisa ser o de um endereco.

5. `std::function` vence claramente quando o conjunto de chamaveis so se conhece na execucao — uma tabela de comandos registrada por plugins, callbacks guardados em objetos de tipos diferentes — porque o template exigiria conhecer todos os tipos na compilacao.

**Autoverificacao:** a mesma lambda nas duas APIs, SBO citado como nao garantido e um caso seu para cada lado da fronteira.
