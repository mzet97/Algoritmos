// Aula 23c C++ — relogios vetoriais, leitura por quorum e token entre nos.
//
// Simulacao deterministica em um processo: 3 replicas com relogio vetorial
// por chave, leitura que consulta a maioria e autenticacao por token com
// comparacao em tempo constante. Alem do primario-replica: o primario
// sozinho responderia o obsoleto em silencio; o quorum detecta o conflito.
//
// Compilar: g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror teoria/cpp/exemplos/23c_relogios.cpp -o /tmp/23c_cpp
// Executar: /tmp/23c_cpp
#include <array>
#include <cassert>
#include <iostream>
#include <map>
#include <string>

namespace {
constexpr int NOS = 3;
constexpr int QUORUM = 2;
const std::string TOKEN = "segredo-de-laboratorio";

struct Entrada {
    int valor = 0;
    std::array<int, NOS> relogio = {0, 0, 0};
    bool presente = false;
};

// Comparacao em tempo constante: sem retorno antecipado no conteudo.
bool token_confere(const std::string& a, const std::string& b)
{
    if (a.size() != b.size()) {
        return false;
    }
    unsigned diff = 0;
    for (std::size_t i = 0; i < a.size(); ++i) {
        diff |= static_cast<unsigned>(a[i] ^ b[i]);
    }
    return diff == 0;
}

struct Replica {
    int id = 0;
    std::map<std::string, Entrada> dados;

    void escrever(const std::string& chave, int valor)
    {
        Entrada& e = dados[chave];
        e.relogio[id] += 1;
        e.valor = valor;
        e.presente = true;
    }

    void receber(const std::string& chave, const Entrada& remota)
    {
        Entrada& local = dados[chave];
        for (int i = 0; i < NOS; ++i) {
            if (remota.relogio[i] > local.relogio[i]) {
                local.relogio[i] = remota.relogio[i];
            }
        }
        local.valor = remota.valor; // ultimo escritor vence; conflito se detecta fora
        local.presente = true;
    }
};

// a domina b se e >= em tudo e > em algo.
bool domina(const std::array<int, NOS>& a, const std::array<int, NOS>& b)
{
    bool maior = false;
    for (int i = 0; i < NOS; ++i) {
        if (a[i] < b[i]) {
            return false;
        }
        maior = maior || (a[i] > b[i]);
    }
    return maior;
}

struct Leitura {
    int valor = 0;
    bool conflito = false;
    bool ausente = true;
};

// Leitura por quorum: consulta todas, exige 2 respostas, escolhe a dominante;
// sem dominante, declara conflito em vez de servir o obsoleto calado.
Leitura ler_quorum(const std::array<Replica, NOS>& replicas, const std::string& chave,
                   const std::string& token)
{
    if (!token_confere(token, TOKEN)) {
        return Leitura{};
    }
    int respostas = 0;
    const Entrada* melhor = nullptr;
    bool conflito = false;
    for (const auto& r : replicas) {
        auto it = r.dados.find(chave);
        if (it == r.dados.end() || !it->second.presente) {
            continue;
        }
        ++respostas;
        if (melhor == nullptr) {
            melhor = &it->second;
        } else if (it->second.relogio == melhor->relogio) {
            continue; // mesma versao replicada: nem domina, nem conflita
        } else if (domina(it->second.relogio, melhor->relogio)) {
            melhor = &it->second;
        } else if (!domina(melhor->relogio, it->second.relogio)) {
            conflito = true;
        }
    }
    if (respostas < QUORUM || melhor == nullptr) {
        return Leitura{};
    }
    return Leitura{melhor->valor, conflito, false};
}
} // namespace

int main()
{
    std::array<Replica, NOS> replicas;
    for (int i = 0; i < NOS; i++) {
        replicas[i].id = i;
    }

    // Fase 1: escrita causal replicada — quorum le sem conflito.
    replicas[0].escrever("k", 111);
    replicas[1].receber("k", replicas[0].dados["k"]);
    replicas[2].receber("k", replicas[0].dados["k"]);
    Leitura l1 = ler_quorum(replicas, "k", TOKEN);
    assert(!l1.ausente && !l1.conflito && l1.valor == 111);
    std::cout << "fase1 quorum limpo: k=" << l1.valor << "\n";

    // Fase 2: particao com escrita bilateral — vetores denunciam concorrencia.
    replicas[0].escrever("k", 222); // lado {0}, relogio (2,0,0)
    replicas[2].escrever("k", 333); // lado {2}, relogio (1,0,2)->(0,0,1)+base
    Leitura l2 = ler_quorum(replicas, "k", TOKEN);
    assert(!l2.ausente && l2.conflito);
    std::cout << "fase2 conflito detectado pelo quorum (primario sozinho serviria calado)\n";

    // Fase 3: token errado nao le nada — autenticacao entre nos.
    Leitura l3 = ler_quorum(replicas, "k", "token-errado");
    assert(l3.ausente);
    std::cout << "fase3 token invalido: leitura recusada\n";

    // Fase 4: cura por anti-entropia com regra publicada (maior relogio total,
    // desempate por id). Todos convergem; a perdedora e nomeada.
    const Entrada* vencedora = &replicas[0].dados["k"];
    int soma_v = vencedora->relogio[0] + vencedora->relogio[1] + vencedora->relogio[2];
    for (const auto& r : replicas) {
        const Entrada& e = r.dados.at("k");
        int soma = e.relogio[0] + e.relogio[1] + e.relogio[2];
        if (soma > soma_v) {
            vencedora = &e;
            soma_v = soma;
        }
    }
    for (auto& r : replicas) {
        r.dados["k"] = *vencedora;
    }
    Leitura l4 = ler_quorum(replicas, "k", TOKEN);
    assert(!l4.ausente && !l4.conflito);
    std::cout << "fase4 pos-cura: k=" << l4.valor << " convergente, conflito resolvido pela regra\n";
    std::cout << "23c ok: vetor detecta, quorum expoe, token protege\n";
    return 0;
}
