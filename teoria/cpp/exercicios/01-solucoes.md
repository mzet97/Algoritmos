---
layout: default
---

# Aula 01 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../01.md#12-exercicios-obrigatorios).

1, 2 e 3. Programa completo:

```cpp
#include <iostream>
#include <string>

int main()
{
    std::string nome = "Ada";
    int idade = 36;
    double altura = 1.68;
    bool estudando_cpp = true;

    std::cout << "nome: " << nome << '\n';
    std::cout << "idade: " << idade << '\n';
    std::cout << "altura: " << altura << '\n';
    std::cout << "estudando_cpp: " << std::boolalpha << estudando_cpp << '\n';
    return 0;
}
```

Trocar os literais (exercicio 2) muda so a saida; sem `std::boolalpha`, o `bool` imprimiria `1`.

4. Mesmas flags nos dois compiladores:

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror -o programa programa.cpp
clang++ -std=c++20 -Wall -Wextra -Wpedantic -Werror -o programa_clang programa.cpp
```

Os dois aceitam este programa sem avisos nesta rodada (g++ 16 e clang++ 22). As mensagens de diagnostico diferem em estilo entre compiladores — comparar os textos, e nao so o codigo de saida, e o exercicio.

5. `main` devolve `int` porque o sistema operacional (e quem lancou o processo: shell, script, servico) recebe um codigo de saida numerico. `0` e a convencao de sucesso; valores diferentes de zero sinalizam classes de falha que scripts testam sem ler a saida — o mesmo contrato cobrado na [aula 19 de C](../../c/exercicios/19-solucoes.md).

**Autoverificacao:** `estudando_cpp: true` com `boolalpha`, compilacao limpa nos dois compiladores e codigo de saida 0 conferido com `echo $?`.
