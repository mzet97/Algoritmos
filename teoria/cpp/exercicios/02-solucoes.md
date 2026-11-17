---
layout: default
---

# Aula 02 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../02.md#14-exercicios-obrigatorios).

```cpp
#include <iostream>
#include <string>

int main()
{
    const int LIMITE = 3;
    for (int i = 0; i < LIMITE; i++) {
        std::cout << "volta " << i << '\n';
    }

    int valor = 10;
    int& ref = valor;
    ref = 42;
    std::cout << "valor apos referencia: " << valor << '\n'; // 42

    const double NOTA_CORTE = 6.0;
    double nota = 7.5;
    std::cout << (nota >= NOTA_CORTE ? "aprovado" : "reprovado") << '\n';

    int mes = 3;
    std::string nome_mes;
    switch (mes) {
    case 1: nome_mes = "janeiro"; break;
    case 2: nome_mes = "fevereiro"; break;
    case 3: nome_mes = "marco"; break;
    case 4: nome_mes = "abril"; break;
    case 5: nome_mes = "maio"; break;
    case 6: nome_mes = "junho"; break;
    case 7: nome_mes = "julho"; break;
    case 8: nome_mes = "agosto"; break;
    case 9: nome_mes = "setembro"; break;
    case 10: nome_mes = "outubro"; break;
    case 11: nome_mes = "novembro"; break;
    case 12: nome_mes = "dezembro"; break;
    default: nome_mes = "invalido"; break;
    }
    std::cout << "mes: " << nome_mes << '\n';
    return 0;
}
```

5. Passar/ guardar **por valor** copia o objeto: o outro lado tem um objeto proprio e mudancas nao voltam. Uma **referencia** e um outro nome para o mesmo objeto: escrever nela escreve no original, nao existe referencia "nula" valida nem reatribuicao para outro objeto depois de ligada. E essa diferenca — copia contra identidade — que decide assinaturas de funcao nas proximas aulas.

**Autoverificacao:** alterar `ref` muda `valor`, nota de corte nomeada e mes fora de 1–12 cai no `default`.
