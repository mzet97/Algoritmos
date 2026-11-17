---
layout: default
---

# Aula 03 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../03.md#17-exercicios-obrigatorios).

```cpp
#include <iostream>

int triplo(int x)
{
    return 3 * x;
}

int maximo(int a, int b)
{
    return a > b ? a : b;
}

double maximo(double a, double b)
{
    return a > b ? a : b;
}

namespace util {

int dobro(int x)
{
    return 2 * x;
}

int quadrado(int x)
{
    return x * x;
}

} // namespace util

int main()
{
    std::cout << "triplo: " << triplo(7) << '\n';       // 21
    std::cout << "max int: " << maximo(4, 9) << '\n';   // 9
    std::cout << "max double: " << maximo(4.5, 9.25) << '\n'; // 9.25
    std::cout << "util: " << util::dobro(5) << ' ' << util::quadrado(5) << '\n';

    if (true) {
        int dentro = 123;
        std::cout << "dentro: " << dentro << '\n';
    }
    // 'dentro' nao existe aqui: o escopo terminou no '}' acima.
    return 0;
}
```

2. Sobrecarga: o compilador escolhe `maximo` pelos tipos dos argumentos da chamada. Com argumentos mistos ambiguos (`maximo(4, 9.5)`), ele recusa em vez de adivinhar — ambiguidade e erro, nao promocao silenciosa.

4. A variavel declarada dentro do bloco `if` vive do ponto da declaracao ate o `}` do bloco; fora, o nome nao existe (erro de compilacao se referenciado), e a memoria dela pode ser reutilizada. Escopo e tempo de vida andam juntos para objetos automaticos.

5. **Declaracao** introduz nome e tipo e pode se repetir (`int maximo(int, int);`). **Definicao** e a declaracao que cria o corpo da funcao ou o armazenamento da variavel — para funcoes e variaveis nao-inline, exatamente uma por programa (ODR). Headers guardam declaracoes; a definicao mora num `.cpp`, como na [aula 12 de C](../../c/exercicios/12-solucoes.md).

**Autoverificacao:** saidas 21, 9, 9.25 e `util: 10 25`; usar `dentro` fora do bloco falha a compilacao, como previsto.
