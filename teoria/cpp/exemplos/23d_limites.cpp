// Aula 23d C++ — limites globais: o servidor inteiro cabe num orçamento.
//
// Tetos por conexão existem na 23a; esta aula impõe os tetos do servidor:
// conexões simultâneas, bytes de entrada por minuto e conexões por IP.
// Sem teto global, 32 clientes dentro do teto individual ainda derrubam
// a máquina somados. O programa simula a admissão e cobra cada recusa.
//
// Compilar: g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror teoria/cpp/exemplos/23d_limites.cpp -o /tmp/23d_cpp
// Executar: /tmp/23d_cpp
#include <cassert>
#include <cstddef>
#include <iostream>
#include <map>
#include <string>

namespace {

struct Orcamento {
    int max_conexoes = 32;
    std::size_t max_bytes_entrada = 1 << 20; // 1 MiB por janela
    int max_por_ip = 4;
    int abertas = 0;
    std::size_t bytes_janela = 0;
    std::map<std::string, int> por_ip;

    bool admitir(const std::string& ip)
    {
        if (abertas >= max_conexoes) {
            return false;
        }
        int& do_ip = por_ip[ip]; // conta o IP antes de decidir
        if (do_ip >= max_por_ip) {
            return false;
        }
        ++abertas;
        ++do_ip;
        return true;
    }

    void liberar(const std::string& ip)
    {
        --abertas;
        auto it = por_ip.find(ip);
        if (it != por_ip.end()) {
            if (--(it->second) == 0) {
                por_ip.erase(it);
            }
        }
    }

    bool consumir_entrada(std::size_t n)
    {
        if (bytes_janela + n > max_bytes_entrada) {
            return false;
        }
        bytes_janela += n;
        return true;
    }

    void nova_janela() { bytes_janela = 0; }
};

} // namespace

int main()
{
    Orcamento orc;

    // Teto de conexoes simultaneas: a 33a e recusada, a liberada abre vaga.
    for (int i = 0; i < 32; ++i) {
        assert(orc.admitir("10.0.0." + std::to_string(i % 8 + 1)));
    }
    assert(!orc.admitir("10.0.0.99")); // 33a simultanea: recusada
    orc.liberar("10.0.0.1");
    assert(orc.admitir("10.0.0.99")); // vaga liberada: admitida
    std::cout << "conexoes: teto global de 32 cobrado, vaga reutilizada\n";

    // Teto por IP: o 5o do mesmo IP cai, mesmo com vagas globais.
    Orcamento orc2;
    assert(orc2.admitir("10.0.0.7"));
    assert(orc2.admitir("10.0.0.7"));
    assert(orc2.admitir("10.0.0.7"));
    assert(orc2.admitir("10.0.0.7"));
    assert(!orc2.admitir("10.0.0.7"));
    std::cout << "por IP: 5a conexao do mesmo IP recusada\n";

    // Teto de bytes por janela: rajada acima de 1 MiB e cortada.
    Orcamento orc3;
    assert(orc3.consumir_entrada(900 << 10));
    assert(!orc3.consumir_entrada(200 << 10)); // estouraria o MiB
    orc3.nova_janela();
    assert(orc3.consumir_entrada(200 << 10)); // janela nova: passa
    std::cout << "bytes: rajada cortada na janela, liberada na proxima\n";

    // Conexao lenta que ocupa slot sem consumir: o teto de conexoes a pega,
    // nao o de bytes — cada abuso cai num teto diferente.
    Orcamento orc4;
    for (int i = 0; i < 32; ++i) {
        assert(orc4.admitir("lento-" + std::to_string(i % 8)));
    }
    assert(!orc4.admitir("vitima")); // lenta ocupou tudo: vitima recusada
    std::cout << "lentas: 32 ociosas bloqueiam a vitima (tempo limite resolve na 23a)\n";
    std::cout << "23d ok: tres tetos, tres abusos recusados\n";
    return 0;
}
