---
layout: default
---

# Aula 08 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../08.md#16-exercicios-obrigatorios).

```cpp
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    std::vector<int> v{5, 2, 8, 2, 9, 1};
    std::sort(v.begin(), v.end());

    for (std::size_t i = 0; i < v.size(); i++) {
        std::cout << i << ':' << v[i] << ' ';
    }
    std::cout << '\n';
    for (int x : v) {
        std::cout << x << ' ';
    }
    std::cout << '\n';

    std::cout << "quantos 2: " << std::count(v.begin(), v.end(), 2) << '\n';

    std::vector<std::string> palavras{"mesa", "cadeira", "porta"};
    auto achou = std::find(palavras.begin(), palavras.end(), "cadeira");
    if (achou != palavras.end()) {
        std::cout << "achou: " << *achou << '\n';
    } else {
        std::cout << "nao achou\n";
    }
    return 0;
}
```

Saida: indices sobre o vetor ja ordenado (`0:1 1:2 2:2 3:5 4:8 5:9`), a mesma sequencia no range-for, `quantos 2: 2` e `achou: cadeira`. Desreferenciar o retorno de `find` sem comparar com `end()` e o erro classico: em caso de ausencia, esse iterador nao aponta para elemento algum.

5. `std::vector` e a escolha inicial melhor que array cru dinamico porque junta dono, tamanho e capacidade num tipo so: copia e movimento corretos (regra do zero da [aula 05](05-solucoes.md)), `size()` sempre certo, crescimento gerenciado e interface comum a todos os algoritmos da biblioteca. Array cru dinamico separa ponteiro, tamanho e dever de liberar em tres variaveis que podem divergir.

**Autoverificacao:** vetor ordenado antes do `count`, `find` conferido contra `end()` e nenhum `new[]`/`delete[]` no programa.
