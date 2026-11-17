---
layout: default
---

# Aula 07 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../07.md#17-exercicios-obrigatorios).

```cpp
#include <concepts>
#include <iostream>
#include <string>
#include <vector>

template <typename T>
T minimo(T a, T b)
{
    return a < b ? a : b;
}

template <typename T>
struct Par {
    T primeiro;
    T segundo;
};

template <std::integral T>
T dobro(T x)
{
    return x * 2;
}

int main()
{
    std::cout << "min int: " << minimo(4, 9) << '\n';
    std::cout << "min double: " << minimo(4.5, 9.25) << '\n';

    Par<std::string> par{"esquerda", "direita"};
    std::cout << "par: " << par.primeiro << '/' << par.segundo << '\n';

    std::vector<int> valores{1, 2, 3};
    auto copia = valores; // auto deduz std::vector<int>, com copia
    auto& ref = valores;  // sem '&', auto copiaria
    ref.push_back(4);
    std::cout << "tamanhos: " << copia.size() << ' ' << valores.size() << '\n';

    std::cout << "dobro: " << dobro(21) << '\n';
    // dobro(2.5) nao compila: double nao satisfaz std::integral.
    return 0;
}
```

3. `auto` nao e tipagem dinamica: o tipo e deduzido uma vez, na declaracao, e conferido estaticamente. O erro classico e esquecer o `&` e copiar sem perceber — `copia` acima fica com 3 elementos enquanto `valores` cresce para 4.

4. O conceito `std::integral` rejeita `double` **na chamada**, com erro apontando a restricao violada, em vez de instanciar o template e falhar dentro do corpo.

5. Na sobrecarga comum, voce escreve cada versao e o compilador escolhe entre funcoes existentes. No template, voce escreve um molde e o compilador **gera** a funcao de cada tipo usado, com cheque completo por instancia — o erro aparece onde o molde encontra um tipo que nao cumpre o que o corpo supoe (daı conceitos para declarar a suposicao).

**Autoverificacao:** `minimo` com dois tipos, `Par<std::string>`, o efeito do `&` no `auto` visivel nos tamanhos e `dobro(2.5)` recusado pelo conceito.
