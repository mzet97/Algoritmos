---
layout: default
---

# Aula 32 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../32.md#18-exercicios-obrigatorios).

```cpp
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <vector>

enum class NivelLog { Info, Erro };
using LogFn = std::function<void(NivelLog, const std::string&)>;

class LojaConfig {
public:
    explicit LojaConfig(LogFn log = nullptr) : log_(std::move(log)) {}

    // Atualiza se a chave existe; copia chave e valor para dentro.
    void define(std::string chave, std::string valor)
    {
        for (auto& par : entradas_) {
            if (par.first == chave) {
                par.second = std::move(valor);
                registra(NivelLog::Info, "atualizada: " + chave);
                return;
            }
        }
        entradas_.emplace_back(std::move(chave), std::move(valor));
    }

    std::optional<std::string> obtem(const std::string& chave) const
    {
        for (const auto& par : entradas_) {
            if (par.first == chave) {
                return par.second;
            }
        }
        return std::nullopt;
    }

    // Politica: true se removeu; false se a chave nao existia (sem excecao).
    bool remove(const std::string& chave)
    {
        for (auto it = entradas_.begin(); it != entradas_.end(); ++it) {
            if (it->first == chave) {
                entradas_.erase(it);
                registra(NivelLog::Info, "removida: " + chave);
                return true;
            }
        }
        registra(NivelLog::Erro, "remocao de ausente: " + chave);
        return false;
    }

    std::size_t tamanho() const { return entradas_.size(); }

    // Devolve quantas linhas ruins foram ignoradas; -1 se nao abriu.
    int carrega(const std::string& caminho)
    {
        std::ifstream f{caminho};
        if (!f) {
            return -1;
        }
        int ignoradas = 0;
        std::string linha;
        while (std::getline(f, linha)) {
            auto igual = linha.find('=');
            if (igual == std::string::npos || igual == 0) {
                ignoradas++;
                continue;
            }
            define(linha.substr(0, igual), linha.substr(igual + 1));
        }
        return ignoradas;
    }

private:
    void registra(NivelLog nivel, const std::string& mensagem) const
    {
        if (log_) {
            log_(nivel, mensagem);
        }
    }

    std::vector<std::pair<std::string, std::string>> entradas_;
    LogFn log_;
};

int main()
{
    int falhas = 0;
    LojaConfig loja{[](NivelLog, const std::string& m) {
        std::cout << "log: " << m << '\n';
    }};
    loja.define("host", "localhost");
    loja.define("porta", "8080");
    loja.define("porta", "9090");
    if (loja.tamanho() != 2) falhas++;
    if (loja.obtem("porta") != std::optional<std::string>{"9090"}) falhas++;
    if (loja.obtem("ausente").has_value()) falhas++;
    if (!loja.remove("host")) falhas++;
    if (loja.remove("host")) falhas++; // ausencia: false, sem excecao

    {
        std::ofstream f{"config_teste.txt"};
        f << "modo=rapido\nruim\nlimite=10\n";
    }
    LojaConfig do_arquivo;
    if (do_arquivo.carrega("config_teste.txt") != 1) falhas++;
    if (do_arquivo.obtem("limite") != std::optional<std::string>{"10"}) falhas++;
    std::remove("config_teste.txt");

    std::cout << "testes: " << (falhas == 0 ? "ok" : "FALHOU") << '\n';
    return falhas == 0 ? 0 : 1;
}
```

5. Copiar na entrada (`std::string` por valor e movida para dentro) faz a loja dona das suas strings: o chamador pode destruir ou reutilizar o buffer dele no retorno da chamada. O contrato fica "passe e esqueca", sem regras escondidas de tempo de vida — o mesmo argumento da [aula 32 de C](../../c/exercicios/32-solucoes.md).

6. Evoluir sem quebrar a fronteira: `define/obtem/remove/carrega` e suas politicas sao o contrato; trocar o `vector` interno por `unordered_map` muda complexidade sem tocar uma linha do cliente — desde que a ordem de iteracao nao tenha virado comportamento prometido. Fronteira estavel e o que permite a troca, como no PIMPL da [aula 26](26-solucoes.md).

**Autoverificacao:** `testes: ok` com atualizacao, ausencia nos dois sentidos, carga com uma linha ignorada e log chegando pelo callback (a loja nao imprime).
