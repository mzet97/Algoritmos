---
layout: default
---

# Aula 09 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../09.md#16-exercicios-obrigatorios).

```cpp
#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

struct SomaFixo {
    int fixo;
    int operator()(int x) const { return x + fixo; }
};

double aplica(const std::function<double(double)>& f, double x)
{
    return f(x);
}

int main()
{
    SomaFixo mais10{10};
    std::cout << "functor: " << mais10(5) << '\n'; // 15

    int contador = 0;
    auto acumula = [&contador](int x) { contador += x; };
    acumula(3);
    acumula(4);
    std::cout << "contador: " << contador << '\n'; // 7

    std::vector<std::string> palavras{"banana", "uva", "abacaxi", "kiwi"};
    std::sort(palavras.begin(), palavras.end(),
              [](const std::string& a, const std::string& b) {
                  return a.size() < b.size();
              });
    for (const auto& p : palavras) {
        std::cout << p << ' ';
    }
    std::cout << '\n'; // uva kiwi banana abacaxi

    double fator = 2.5;
    auto vezes = [fator](double x) { return x * fator; };
    std::cout << "function: " << aplica(vezes, 4.0) << '\n'; // 10
    return 0;
}
```

4. O ponteiro de funcao so aceita funcoes livres ou lambdas **sem captura**: nao ha onde guardar o `fator`. `std::function` apaga o tipo concreto e guarda qualquer chamavel com aquela assinatura — ao preco de uma possivel alocacao e chamada indireta.

5. Functor: objeto com `operator()` — tem estado e tipo proprio, o compilador pode embutir a chamada. Lambda: sintaxe para gerar um functor anonimo com capturas declaradas. Ponteiro de funcao: so endereco de codigo, sem estado e sem tipo por chamavel. A progressao e de flexibilidade crescente e previsibilidade decrescente de custo.

**Autoverificacao:** contador acumulado ate 7 pela captura por referencia, ordenacao por tamanho estavel na saida e `std::function` recebendo lambda com captura.
