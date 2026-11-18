// Aula 23f C++ — eleicao, commit por maioria e RPC idempotente.
//
// O que a 23c protegeu na leitura, esta aula garante na escrita: lider
// eleito por maioria, comando confirmado so na maioria, repeticao servida
// do cache (inclusive o erro). Simulacao deterministica em um processo.
//
// Compilar: g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror teoria/cpp/exemplos/23f_commit.cpp -o /tmp/23f_cpp
// Executar: /tmp/23f_cpp
#include <array>
#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace {

constexpr int NOS = 3;
constexpr int QUORUM = 2;

enum class Papel { Seguidor, Candidato, Lider };

struct Entrada {
    int termo = 0;
    std::string comando;
};

struct No {
    int termo = 0;
    Papel papel = Papel::Seguidor;
    int votou_em = -1;
    std::vector<Entrada> log;
    int commit = -1;
    bool vivo = true;
};

struct Resposta {
    std::string saida;
    bool repeticao = false;
    bool erro = false;
};

// Eleicao num subconjunto (particao): maioria elege, minoria nao.
int eleger(std::array<No, NOS>& nos, const std::vector<int>& membros)
{
    int cand = membros[0];
    No& c = nos[cand];
    c.termo += 1;
    c.papel = Papel::Candidato;
    c.votou_em = cand;
    for (int id : membros) {
        nos[id].termo = c.termo;
    }
    int votos = 1;
    for (std::size_t i = 1; i < membros.size(); ++i) {
        No& e = nos[membros[i]];
        // Regra de atualizacao: voto so em candidato pelo menos tao
        // atualizado quanto o eleitor (aqui, tamanho do log como relogio).
        if (e.votou_em == -1 && (int)c.log.size() >= (int)e.log.size()) {
            e.votou_em = cand;
            ++votos;
        }
    }
    if (votos >= QUORUM) {
        c.papel = Papel::Lider;
        return cand;
    }
    c.papel = Papel::Seguidor;
    return -1;
}

// Replica para os vivos do subconjunto; confirma so com maioria.
bool replicar(std::array<No, NOS>& nos, int lider, const std::vector<int>& membros,
              const std::string& comando)
{
    No& l = nos[lider];
    assert(l.papel == Papel::Lider);
    l.log.push_back({l.termo, comando});
    int acks = 1;
    for (int id : membros) {
        if (id == lider || !nos[id].vivo) {
            continue;
        }
        nos[id].log.push_back({l.termo, comando});
        ++acks;
    }
    if (acks >= QUORUM) {
        l.commit = (int)l.log.size() - 1;
        for (int id : membros) {
            if (nos[id].vivo && nos[id].termo == l.termo) {
                nos[id].commit = l.commit;
            }
        }
        return true;
    }
    return false;
}

// Servidor RPC idempotente: id repetido devolve o gravado, sem reexecutar.
struct Servidor {
    std::map<std::string, Resposta> cache;
    int execucoes = 0;

    Resposta servir(const std::string& id, const std::string& metodo)
    {
        if (id.empty()) {
            return {"sem id: rejeitado", false, true};
        }
        auto it = cache.find(id);
        if (it != cache.end()) {
            Resposta r = it->second;
            r.repeticao = true;
            return r;
        }
        ++execucoes;
        Resposta r = (metodo == "ping") ? Resposta{"pong", false, false}
                                        : Resposta{"metodo invalido", false, true};
        cache[id] = r;
        return r;
    }
};

} // namespace

int main()
{
    std::array<No, NOS> nos;
    const std::vector<int> todos = {0, 1, 2};

    int lider = eleger(nos, todos);
    assert(lider == 0);
    assert(replicar(nos, lider, todos, "op-1"));
    std::cout << "fase1 lider eleito, op-1 confirmada por maioria\n";

    // Particao: minoria de 1 nao elege; escrita solitaria nao confirma.
    assert(eleger(nos, std::vector<int>{2}) == -1); // minoria nao elege
    nos[2].log.push_back({nos[2].termo, "op-solitária"});
    assert(nos[2].commit == 0);
    std::cout << "fase2 minoria sem lider; escrita solitaria nao confirmada\n";

    // Maioria restante confirma sem o isolado.
    nos[2].vivo = false;
    assert(replicar(nos, 0, {0, 1}, "op-2"));
    nos[2].vivo = true;
    std::cout << "fase3 maioria confirma op-2 sem o isolado\n";

    // Cura com termo novo: log confirmado sobrescreve o nao confirmado.
    for (auto& n : nos) {
        n.papel = Papel::Seguidor;
        n.votou_em = -1;
    }
    lider = eleger(nos, todos);
    assert(lider != -1);
    for (auto& n : nos) {
        n.log = nos[lider].log;
        n.commit = nos[lider].commit;
    }
    assert(nos[2].log.size() == 2 && nos[2].log[1].comando == "op-2");
    std::cout << "fase4 pos-cura: op-solitaria descartada, op-2 confirmada\n";

    // RPC idempotente sobre o commit: repeticao nao reexecuta,
    // erro memorizado tambem nao.
    Servidor srv;
    Resposta r1 = srv.servir("req-1", "ping");
    assert(!r1.erro && !r1.repeticao && r1.saida == "pong");
    Resposta r2 = srv.servir("req-1", "ping");
    assert(r2.repeticao && r2.saida == "pong" && srv.execucoes == 1);
    Resposta r3 = srv.servir("req-2", "voar");
    assert(r3.erro && !r3.repeticao);
    Resposta r4 = srv.servir("req-2", "voar");
    assert(r4.repeticao && r4.erro && srv.execucoes == 2);
    Resposta r5 = srv.servir("", "ping");
    assert(r5.erro);
    std::cout << "rpc: repeticao do cache, erro memorizado, sem id recusado\n";
    std::cout << "23f ok: eleicao, commit por maioria, RPC idempotente\n";
    return 0;
}
