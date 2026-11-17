---
layout: default
---

# Aula 12 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../12.md#16-exercicios-obrigatorios).

1 e 2. Programa em tres arquivos:

```cpp
// geometria.h
#ifndef GEOMETRIA_H
#define GEOMETRIA_H

double area_retangulo(double base, double altura);

template <typename T>
T maior(T a, T b)
{
    return a > b ? a : b;
}

#endif
```

```cpp
// geometria.cpp
#include "geometria.h"

double area_retangulo(double base, double altura)
{
    return base * altura;
}
```

```cpp
// main.cpp
#include <iostream>
#include "geometria.h"

int main()
{
    std::cout << "area: " << area_retangulo(3.0, 4.0) << '\n';
    std::cout << "maior: " << maior(7, 12) << '\n';
    return 0;
}
```

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror -c geometria.cpp -o geometria.o
g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror -c main.cpp -o main.o
g++ geometria.o main.o -o programa
./programa   # area: 12 / maior: 12
```

O template `maior` vive no header porque a definicao precisa estar visivel onde ele e instanciado; a funcao comum vive num `.cpp` so, e o header so a declara.

3. Remover `geometria.cpp` da ligacao produz o classico `undefined reference to 'area_retangulo(double, double)'` **na ligacao**, com a compilacao de `main.cpp` limpa: declarar basta para compilar, nao para ligar. O nome decorado na mensagem ja e a ponte para o item 5.

4. ODR (One Definition Rule): cada funcao ou variavel nao-inline tem exatamente uma definicao no programa inteiro. Violar sem o ligador perceber (duas definicoes diferentes do mesmo nome em `.cpp` distintos) e comportamento indefinido sem diagnostico garantido — o programa liga e usa "uma das" versoes.

5. ABI em C++ inclui tudo o que a [aula 12 de C](../../c/exercicios/12-solucoes.md) listou (tamanhos, alinhamento, convencao de chamada) **mais** o esquema de decoracao de nomes (name mangling), layout de classes e de tabelas virtuais, que variam entre compiladores e versoes. Por isso biblioteca C++ pre-compilada amarra compilador e flags de um jeito que biblioteca C nao amarra.

**Autoverificacao:** `area: 12 / maior: 12`, erro de ligacao reproduzido e desfeito, e template no header com a razao escrita por voce.
