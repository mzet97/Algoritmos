---
layout: default
---

# Aula 17 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../17.md#17-exercicios-obrigatorios).

1, 2 e 5. API C sobre classe C++, com handle opaco e excecoes convertidas em codigo:

```cpp
// contador_api.h (compativel com C)
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Contador Contador; // opaco

// 0 = sucesso; codigos negativos = classes de erro documentadas
Contador* contador_create(int limite);
void contador_destroy(Contador* c);
int contador_incrementa(Contador* c); // 1 ao atingir o limite; -1 handle nulo
int contador_valor(const Contador* c); // -1 handle nulo

#ifdef __cplusplus
}
#endif
```

```cpp
// contador_api.cpp
#include "contador_api.h"

#include <new>
#include <stdexcept>

class ContadorInterno {
public:
    explicit ContadorInterno(int limite) : limite_(limite)
    {
        if (limite <= 0) {
            throw std::invalid_argument("limite deve ser positivo");
        }
    }
    int incrementa()
    {
        valor_++;
        return valor_ == limite_ ? 1 : 0;
    }
    int valor() const { return valor_; }

private:
    int limite_;
    int valor_ = 0;
};

struct Contador {
    ContadorInterno interno;
};

extern "C" {

Contador* contador_create(int limite)
{
    try {
        return new Contador{ContadorInterno{limite}};
    } catch (...) {
        return nullptr; // excecao vira valor na fronteira
    }
}

void contador_destroy(Contador* c)
{
    delete c; // delete de nullptr nao faz nada
}

int contador_incrementa(Contador* c)
{
    if (c == nullptr) {
        return -1;
    }
    try {
        return c->interno.incrementa();
    } catch (...) {
        return -2;
    }
}

int contador_valor(const Contador* c)
{
    return c == nullptr ? -1 : c->interno.valor();
}

} // extern "C"
```

```cpp
// main.cpp
#include <iostream>
#include "contador_api.h"

int main()
{
    Contador* c = contador_create(3);
    if (c == nullptr) {
        return 1;
    }
    std::cout << contador_incrementa(c) << '\n'; // 0
    std::cout << contador_incrementa(c) << '\n'; // 0
    std::cout << contador_incrementa(c) << '\n'; // 1 (limite)
    std::cout << "valor: " << contador_valor(c) << '\n';
    std::cout << "invalido: " << contador_incrementa(nullptr) << '\n'; // -1
    contador_destroy(c);
    contador_destroy(nullptr);

    Contador* ruim = contador_create(0); // excecao interna vira nullptr
    std::cout << "create invalido: " << (ruim == nullptr ? "nulo" : "vivo") << '\n';
    return 0;
}
```

3. O compilador C++ decora nomes com os tipos (name mangling) para permitir sobrecarga; um cliente C procura o nome simples. `extern "C"` desliga a decoracao na fronteira — ver tambem a [aula 12](12-solucoes.md) e a [aula 18 de C](../../c/exercicios/18-solucoes.md).

4. `std::string` nao e boa fronteira porque seu layout (ponteiro/tamanho/capacidade ou buffer interno SSO), sua ABI e seu alocador pertencem a uma implementacao e versao de C++ especificas. Do lado C (ou de outra linguagem), nao ha contrato estavel para ler ou liberar aquilo. Fronteira boa usa tipos com layout declarado: `const char*` + tamanho, inteiros de largura fixa e handles opacos.

5. A regra e: nenhuma excecao atravessa. Cada funcao exportada envolve o corpo em `try/catch`, traduz excecoes conhecidas para codigos documentados e usa `catch (...)` como ultima linha, devolvendo um codigo generico — porque uma excecao C++ escapando para um chamador C (ou para dentro de um callback do sistema) termina em `std::terminate` ou comportamento indefinido.

**Autoverificacao:** cliente usa so o header C, `create(0)` devolve nulo sem excecao escapar e `nm` no objeto mostra `contador_create`, `contador_destroy`, `contador_incrementa` e `contador_valor` sem decoracao (os metodos internos da classe, que nao atravessam a fronteira, continuam decorados).
