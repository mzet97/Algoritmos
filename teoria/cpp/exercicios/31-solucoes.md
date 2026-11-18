---
layout: default
---

# Aula 31 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../31.md#18-exercicios-obrigatorios). Recorte executavel: parser separado como funcao pura testavel, `UPPER` e a politica de saida — o servidor completo endurecido, com timeout por conexao e testes adversariais, e a ponte [23a](../23a.md):

```cpp
#include <algorithm>
#include <cctype>
#include <iostream>
#include <optional>
#include <string>

// Parser de linhas: acumula bytes e devolve a proxima linha completa,
// ou nullopt se ainda nao chegou o '\n'. Nao conhece sockets.
struct ParserLinhas {
    std::string pendente;

    std::optional<std::string> alimenta(const char* dados, std::size_t n)
    {
        pendente.append(dados, n);
        auto pos = pendente.find('\n');
        if (pos == std::string::npos) {
            return std::nullopt;
        }
        std::string linha = pendente.substr(0, pos);
        pendente.erase(0, pos + 1);
        return linha;
    }
};

std::string despacha(const std::string& linha)
{
    if (linha == "PING") {
        return "PONG\n";
    }
    if (linha.rfind("UPPER ", 0) == 0) {
        std::string texto = linha.substr(6);
        std::transform(texto.begin(), texto.end(), texto.begin(),
                       [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
        return texto + "\n";
    }
    return "ERRO comando desconhecido\n";
}

int main()
{
    int falhas = 0;
    ParserLinhas p;
    if (p.alimenta("UP", 2).has_value()) falhas++;
    auto linha = p.alimenta("PER ola\n", 9);
    if (!linha || despacha(*linha) != "OLA\n") falhas++;
    if (despacha("PING") != "PONG\n") falhas++;
    if (despacha("XYZ") != "ERRO comando desconhecido\n") falhas++;
    std::cout << "testes do parser/despacho: " << (falhas == 0 ? "ok" : "FALHOU") << '\n';
    return falhas == 0 ? 0 : 1;
}
```

Separar o parser muda o que da para testar: fragmentacao (`"UP"` + `"PER ola\n"`) vira um teste de unidade sem rede, e o servidor da 23a consome exatamente "acumule ate a linha" como uma caixa com contrato.

2. A fila de saida (mensagens + deslocamento da atual) ja esta executada na [solucao da aula 23](23-solucoes.md); trocar o buffer unico por ela e o que permite responder varias linhas sem esperar a drenagem de cada uma.

3. Timeout por conexao ociosa e o mecanismo executado na ponte [23a](../23a.md): cada conexao guarda o instante do ultimo progresso e o loop encerra quem estoura o prazo — sem isso, um cliente que para de ler segura buffer e fd para sempre.

5. Backpressure continua sendo a politica devedora mesmo no servidor endurecido: a fila de saida de um cliente lento precisa de um **teto** (bytes ou mensagens) e de uma decisao ao estourar — fechar a conexao, descartar ou bloquear o produtor. Sem teto, um cliente que nao le transforma o servidor em alocador infinito; a versao de producao declara esse numero, nao descobre em incidente.

6. Metricas nos pontos de decisao (aceites, recusas por teto, timeouts, erros por comando) e um span por atendimento de conexao, no desenho da [aula 27](27-solucoes.md).

**Autoverificacao:** testes do parser verdes incluindo a linha fragmentada, `UPPER` sobre a linha enquadrada e um teto de fila com a decisao de estouro escrita.
