---
layout: default
---

# Aula 16 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../16.md#15-exercicios-obrigatorios).

1–4. CLI de configuracao completa (arquivo ou `stdin`, vazias ignoradas, chave com espaco rejeitada, contadores no fim):

```cpp
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    std::ifstream arquivo;
    std::istream* entrada = &std::cin;
    if (argc > 2) {
        std::cerr << "uso: " << argv[0] << " [ARQUIVO]\n";
        return 2;
    }
    if (argc == 2) {
        arquivo.open(argv[1]);
        if (!arquivo) {
            std::cerr << "erro: nao foi possivel abrir " << argv[1] << '\n';
            return 1;
        }
        entrada = &arquivo;
    }

    long validas = 0, invalidas = 0, vazias = 0;
    std::string linha;
    while (std::getline(*entrada, linha)) {
        if (linha.find_first_not_of(" \t\r") == std::string::npos) {
            vazias++;
            continue;
        }
        auto igual = linha.find('=');
        if (igual == std::string::npos || igual == 0) {
            invalidas++;
            continue;
        }
        std::string chave = linha.substr(0, igual);
        if (chave.front() == ' ' || chave.front() == '\t') {
            invalidas++; // politica: rejeita, nao apara
            continue;
        }
        std::cout << chave << '=' << linha.substr(igual + 1) << '\n';
        validas++;
    }
    std::cerr << "validas=" << validas << " invalidas=" << invalidas
              << " vazias=" << vazias << '\n';
    return invalidas == 0 ? 0 : 1;
}
```

Os contadores vao para `stderr` justamente para nao contaminar a saida de dados em `stdout`: um pipeline pode consumir os pares enquanto o operador le o resumo.

5. Em infraestrutura, quem chama a CLI e outro programa: ele decide pelo **codigo de saida** (sucesso, uso errado, falha) e arquiva o `stderr` como diagnostico. Misturar erro em `stdout` corrompe o dado que o proximo estagio consome, e sair 0 com erro imprime sucesso falso em todo pipeline que confia no contrato — o mesmo principio executado na [aula 19 de C](../../c/exercicios/19-solucoes.md) e na [aula 31 de Python](../../python/31.md).

**Autoverificacao:** entrada mista imprime so pares validos em `stdout`, resumo com os tres contadores em `stderr`, codigo 1 havendo invalidas e leitura de `stdin` sem argumento.
