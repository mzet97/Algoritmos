---
layout: default
---

# Aula 11 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../11.md#15-exercicios-obrigatorios). Compile com `-std=c++23` (a excecao declarada da aula para `std::expected`); o restante do livro usa C++20.

```cpp
#include <expected>
#include <iostream>
#include <optional>
#include <string>
#include <variant>
#include <vector>

std::optional<int> procura(const std::vector<int>& v, int alvo)
{
    for (int x : v) {
        if (x == alvo) {
            return x;
        }
    }
    return std::nullopt;
}

std::expected<int, std::string> parse_inteiro(const std::string& texto)
{
    try {
        std::size_t usadas = 0;
        int valor = std::stoi(texto, &usadas);
        if (usadas != texto.size()) {
            return std::unexpected("lixo apos o numero");
        }
        return valor;
    } catch (...) {
        return std::unexpected("nao e um inteiro");
    }
}

int main()
{
    std::vector<int> v{3, 7, 11};
    if (auto achou = procura(v, 7)) {
        std::cout << "achou: " << *achou << '\n';
    }
    if (!procura(v, 99)) {
        std::cout << "ausente sem excecao\n";
    }

    if (auto numero = parse_inteiro("42")) {
        std::cout << "parse: " << *numero << '\n';
    }
    if (auto ruim = parse_inteiro("4x"); !ruim) {
        std::cout << "erro de parse: " << ruim.error() << '\n';
    }

    std::variant<int, double, std::string> valor = std::string{"texto"};
    std::visit([](const auto& x) { std::cout << "visita: " << x << '\n'; }, valor);
    valor = 2.5;
    std::visit([](const auto& x) { std::cout << "visita: " << x << '\n'; }, valor);
    return 0;
}
```

4. Prefira `expected` quando o erro e um resultado comum do dominio (parse de entrada do usuario, busca, validacao) e quem chama esta perto e deve decidir. Prefira excecao quando a falha e excepcional, precisa atravessar camadas que nao sabem trata-la e o codigo do caminho feliz ficaria ilegivel propagando erros na mao. Os dois sao contratos; o problema e o terceiro caminho — ignorar retorno e sentinelas magicas.

5. Quando uma excecao desenrola a pilha, cada objeto automatico do caminho e destruido na ordem inversa da construcao. Se cada recurso vive dentro de um dono RAII ([aula 04](04-solucoes.md)), a liberacao acontece em todos os caminhos sem `try/catch` de limpeza — excecao e RAII foram desenhados um para o outro; `new`/`delete` manual e que quebra o par.

**Autoverificacao:** ausencia sem valor sentinela, erro de parse lido via `.error()` e `visit` cobrindo os tres tipos do `variant`.
