---
layout: default
---

# Aula 05 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../05.md#19-exercicios-obrigatorios).

1 e 2. O defeito da copia rasa e o diario das operacoes especiais:

```cpp
#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

class BufferRuim {
public:
    explicit BufferRuim(std::size_t n) : dados_(new int[n]{}), n_(n) {}
    ~BufferRuim() { delete[] dados_; }
    std::size_t tamanho() const { return n_; }
    // Sem copia declarada: a copia padrao copia o MESMO ponteiro.
private:
    int* dados_;
    std::size_t n_;
};

class Diario {
public:
    Diario() { std::cout << "construtor\n"; }
    Diario(const Diario&) { std::cout << "copia-construcao\n"; }
    Diario& operator=(const Diario&) { std::cout << "copia-atribuicao\n"; return *this; }
    Diario(Diario&&) noexcept { std::cout << "movimento-construcao\n"; }
    Diario& operator=(Diario&&) noexcept { std::cout << "movimento-atribuicao\n"; return *this; }
    ~Diario() { std::cout << "destrutor\n"; }
};

Diario fabrica()
{
    return Diario{};
}

int main()
{
    Diario a;
    Diario b = a;          // copia-construcao
    Diario c;
    c = a;                 // copia-atribuicao
    Diario d = fabrica();  // movimento (ou elisao) na construcao
    c = fabrica();         // movimento-atribuicao
    std::vector<int> v{1, 2, 3}; // exercicio 3: dono pronto, zero operacoes suas
    std::cout << "vector tamanho: " << v.size() << '\n';
    return 0;
}
```

`BufferRuim b2 = b1;` deixa dois objetos com o mesmo `dados_`; quando ambos morrem, o mesmo array e deletado duas vezes — comportamento indefinido. As correcoes sao proibir a copia, copia-la de verdade (alocar e copiar elementos) ou trocar o membro por um dono pronto.

3. Com `std::vector<int>`, construtor de copia, atribuicao, movimento e destrutor vem corretos da biblioteca: o tipo dono de array cru de dezenas de linhas vira um membro so, e a regra aplicada e a do zero abaixo.

4. Construtor de copia **cria** um objeto a partir de outro (`Diario b = a;`): nao ha objeto anterior para limpar. Atribuicao por copia opera entre dois objetos vivos (`c = a;`): precisa liberar/substituir o estado antigo de `c` e lidar com autoatribuicao.

5. **Regra dos 3:** se voce escreve destrutor, copia-construcao ou copia-atribuicao na mao, provavelmente precisa dos tres. **Regra dos 5:** idem incluindo as duas operacoes de movimento. **Regra do 0:** o desenho preferido — compor membros que ja sao donos (`vector`, `string`, `unique_ptr`) e nao declarar nenhuma das cinco.

**Autoverificacao:** o diario mostra construcao, as duas copias e pelo menos um movimento antes dos destrutores, e o seu texto explica o double-free do `BufferRuim` sem rodar o programa defeituoso.
