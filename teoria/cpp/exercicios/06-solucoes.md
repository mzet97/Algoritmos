---
layout: default
---

# Aula 06 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../06.md#20-exercicios-obrigatorios).

```cpp
#include <iostream>
#include <memory>
#include <utility>

void consome(std::unique_ptr<int> p)
{
    std::cout << "funcao recebeu: " << *p << '\n';
} // o int e deletado aqui, no fim da funcao dona

int main()
{
    int automatico = 7;
    int* observador = &automatico; // nao e dono: nunca delete
    std::cout << "observado: " << *observador << '\n';

    auto dono = std::make_unique<int>(42);
    consome(std::move(dono));
    std::cout << "dono apos mover: " << (dono == nullptr ? "nulo" : "vivo") << '\n';

    auto a = std::make_shared<int>(5);
    {
        auto b = a;
        std::cout << "use_count dentro: " << a.use_count() << '\n'; // 2
    }
    std::cout << "use_count fora: " << a.use_count() << '\n'; // 1
    return 0;
}
```

3. Os dois `shared_ptr` compartilham o mesmo objeto e o mesmo bloco de controle: o objeto morre quando o **ultimo** deles sai de escopo, nao quando qualquer um sai. Contagem compartilhada custa atomicos e admite ciclos que vazam (aula 15 de Rust trata do analogo); por isso `shared_ptr` nao e o padrao — `unique_ptr` e.

4. Com `new`/`delete` manuais, entre a alocacao e a liberacao cabe qualquer retorno antecipado ou excecao que pule o `delete`, alem do risco de `delete` errado (simples contra `[]`) e duplo. `make_unique` cria o objeto ja dentro do dono: daquela linha em diante, toda saida de escopo libera, sem codigo seu.

5. Referencia: outro nome, nunca nula por contrato, nao libera. Ponteiro observador (`T*` sem posse, `std::string_view` para texto): pode ser nulo e pendurar — quem usa garante a vida. Ponteiro dono (`unique_ptr`, `shared_ptr`): define quem libera e quando; mover transfere esse dever.

**Autoverificacao:** `dono` nulo apos o `std::move`, `use_count` 2 → 1 e nenhum `delete` escrito por voce.
