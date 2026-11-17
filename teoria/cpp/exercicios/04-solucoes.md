---
layout: default
---

# Aula 04 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../04.md#16-exercicios-obrigatorios).

```cpp
#include <iostream>
#include <string>
#include <utility>

class Produto {
public:
    Produto(std::string nome, double preco)
        : nome_(std::move(nome)), preco_(preco)
    {
    }

    void mostrar() const
    {
        std::cout << nome_ << " custa " << preco_ << '\n';
    }

private:
    std::string nome_;
    double preco_;
};

class Recurso {
public:
    Recurso()
    {
        std::cout << "recurso adquirido\n";
    }

    ~Recurso()
    {
        std::cout << "recurso liberado\n";
    }
};

int main()
{
    Produto p{"teclado", 199.90};
    p.mostrar();

    std::cout << "antes do bloco\n";
    {
        Recurso r;
        std::cout << "dentro do bloco\n";
    } // ~Recurso() roda aqui, sem ninguem chamar
    std::cout << "depois do bloco\n";
    return 0;
}
```

Saida ordenada: `teclado custa 199.9`, `antes do bloco`, `recurso adquirido`, `dentro do bloco`, `recurso liberado`, `depois do bloco`. O ponto do exercicio 4 e a posicao de "recurso liberado": ela sai **no fechamento do escopo**, nao no fim do programa.

5. RAII (Resource Acquisition Is Initialization): a posse de um recurso (memoria, arquivo, socket, trava) fica amarrada ao tempo de vida de um objeto. Adquire no construtor, libera no destrutor — e como o destrutor roda em toda saida de escopo, incluindo retorno antecipado e excecao, a liberacao nao depende de o programador lembrar de cada caminho. `std::string`, `std::vector` e `std::unique_ptr` sao esse padrao na biblioteca padrao.

**Autoverificacao:** `mostrar()` chamavel em objeto `const`, mensagem do destrutor entre "dentro" e "depois" e nenhum `new`/`delete` manual no programa.
