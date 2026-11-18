// Aula 23e C++ — fuzzing sistematico do parser de framing (deterministico).
//
// O parser implementa as regras da 23a: linha com teto de 4 KiB, entrada
// total com teto de 8 KiB, NUL embutido rejeitado, resto = incompleta.
// O fuzzer e um mutador com semente fixa: as mesmas 200 mil entradas em
// toda execucao — reproduzivel por construcao, sem servidor e sem rede.
// Violações viram assert; memoria, o ASan/UBSan acusa.
//
// Compilar: clang++ -std=c++20 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-omit-frame-pointer teoria/cpp/exemplos/23e_fuzz.cpp -o /tmp/23e_fuzz
// Executar: /tmp/23e_fuzz
// (g++ compila igual; o veredito de memoria vem dos sanitizers do clang.)
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace {

constexpr std::size_t LINHA_MAX = 4096;
constexpr std::size_t ENTRADA_MAX = 8192;

enum class Veredito { Ok, Incompleta, Erro };

struct Resultado {
    Veredito veredito;
    std::string linha; // valida so quando veredito == Ok
};

Resultado parse_frame(const std::vector<uint8_t>& buf)
{
    if (buf.size() > ENTRADA_MAX) {
        return {Veredito::Erro, {}}; // entrada total acima do teto
    }
    for (std::size_t i = 0; i < buf.size(); ++i) {
        if (buf[i] == 0) {
            return {Veredito::Erro, {}}; // NUL embutido: framing, nao string C
        }
        if (buf[i] == '\n') {
            if (i + 1 > LINHA_MAX) {
                return {Veredito::Erro, {}}; // linha acima do teto
            }
            return {Veredito::Ok, std::string(buf.begin(), buf.begin() + i)};
        }
    }
    return {Veredito::Incompleta, {}};
}

// xorshift64 com semente fixa: a sequencia e sempre a mesma.
struct Gerador {
    uint64_t estado = 0x123456789abcdefULL;
    uint64_t proximo()
    {
        estado ^= estado << 13;
        estado ^= estado >> 7;
        estado ^= estado << 17;
        return estado;
    }
};

std::vector<uint8_t> mutar(const std::vector<uint8_t>& base, Gerador& g)
{
    static const std::array<std::vector<uint8_t>, 6> pecas = {
        std::vector<uint8_t>{'P', 'I', 'N', 'G', '\n'},
        std::vector<uint8_t>{'Q', 'U', 'I', 'T', '\n'},
        std::vector<uint8_t>(5000, 'A'), // linha acima do teto
        std::vector<uint8_t>{'A', 0, 'B', '\n'}, // NUL embutido
        std::vector<uint8_t>{'P', 'I', 'N', 'G'}, // sem newline: incompleta
        std::vector<uint8_t>(9000, 'B'), // entrada acima do teto
    };
    std::vector<uint8_t> saida = base;
    switch (g.proximo() % 4) {
    case 0: { // emenda peca
        const auto& peca = pecas[g.proximo() % pecas.size()];
        saida.insert(saida.end(), peca.begin(), peca.end());
        break;
    }
    case 1: { // trunca
        if (!saida.empty()) {
            saida.resize(g.proximo() % saida.size());
        }
        break;
    }
    case 2: { // troca byte
        if (!saida.empty()) {
            saida[g.proximo() % saida.size()] = (uint8_t)(g.proximo() & 0xFF);
        }
        break;
    }
    default: { // peca pura como entrada
        saida = pecas[g.proximo() % pecas.size()];
        break;
    }
    }
    return saida;
}

} // namespace

int main()
{
    const std::vector<std::vector<uint8_t>> corpus = {
        {'P', 'I', 'N', 'G', '\n'},
        {},
        {'\n'},
    };
    long oks = 0, incompletas = 0, erros = 0;
    Gerador g;
    std::vector<uint8_t> entrada = corpus[0];
    constexpr long ITERACOES = 200000;
    for (long i = 0; i < ITERACOES; ++i) {
        entrada = mutar(i < 3 ? corpus[(std::size_t)i] : entrada, g);
        Resultado r = parse_frame(entrada);
        // Invariantes do protocolo, cobradas a cada entrada:
        if (r.veredito == Veredito::Ok) {
            ++oks;
            assert(r.linha.size() + 1 <= LINHA_MAX); // Ok nunca acima do teto
            assert(r.linha.find('\0') == std::string::npos); // Ok sem NUL
        } else if (r.veredito == Veredito::Incompleta) {
            ++incompletas;
            assert(entrada.size() <= ENTRADA_MAX); // incompleta cabe no teto
        } else {
            ++erros;
        }
        // Determinismo: reexecucao da mesma entrada da o mesmo veredito.
        Resultado r2 = parse_frame(entrada);
        assert(r2.veredito == r.veredito && r2.linha == r.linha);
    }
    std::cout << "fuzz: " << ITERACOES << " entradas (ok=" << oks
              << " incompletas=" << incompletas << " erros=" << erros << ")\n";
    assert(oks > 1000 && incompletas > 1000 && erros > 1000); // cobriu as 3 classes
    std::cout << "classes cobertas: Ok, Incompleta, Erro — 0 violacoes\n";
    std::cout << "23e ok: parser sob mutacao deterministica, memoria sob sanitizer\n";
    return 0;
}
