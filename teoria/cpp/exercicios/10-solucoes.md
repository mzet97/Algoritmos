---
layout: default
---

# Aula 10 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../10.md#17-exercicios-obrigatorios).

```cpp
#include <iostream>
#include <memory>
#include <vector>

class Dispositivo {
public:
    virtual ~Dispositivo() = default;
    virtual void ligar() = 0;
};

class Lampada : public Dispositivo {
public:
    void ligar() override { std::cout << "lampada acendeu\n"; }
};

class Motor : public Dispositivo {
public:
    void ligar() override { std::cout << "motor girou\n"; }
    ~Motor() override { std::cout << "motor destruido\n"; }
};

int main()
{
    std::vector<std::unique_ptr<Dispositivo>> dispositivos;
    dispositivos.push_back(std::make_unique<Lampada>());
    dispositivos.push_back(std::make_unique<Motor>());
    for (const auto& d : dispositivos) {
        d->ligar();
    }
    return 0; // destrutores das derivadas rodam via base virtual
}
```

Saida: `lampada acendeu`, `motor girou`, `motor destruido`. A ultima linha so aparece porque `~Dispositivo` e virtual.

4. Deletar um `Motor` atraves de `Dispositivo*` sem destrutor virtual na base e comportamento indefinido na pratica que interessa: o destrutor da derivada pode nao rodar, e recursos dela vazam. Com a base usada polimorficamente para deletar, destrutor virtual e parte do contrato, nao decoracao.

5. Polimorfismo **estatico**: templates e sobrecargas — a escolha acontece na compilacao, sem custo de execucao e com tipos concretos conhecidos (a CrTP e a ponte [03a](03a_pistas.md) usam essa via). **Dinamico**: funcoes virtuais — a escolha acontece na execucao pelo tipo real do objeto, o que permite colecoes heterogeneas como a deste exercicio ao custo de indirecao e objetos sempre por referencia/ponteiro.

**Autoverificacao:** a base nao pode ser instanciada, `override` em todas as sobrescritas e o destrutor da derivada aparece ao destruir pela base.
