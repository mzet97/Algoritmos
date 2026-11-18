// Aula 13b C++ — fronteira FFI e ameacas formais: o contrato antes do simbolo.
//
// Uma API C++ exposta como `extern "C"` para C/Rust/Python: a fronteira
// traduz excecao em codigo, valida tudo que atravessa e documenta quem e
// dono de cada byte. Dentro, RAII e excecoes; fora, codigos e ponteiros
// opacos. Excecao que atravessa FFI e comportamento indefinido.
//
// Compilar: g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror teoria/cpp/exemplos/13b_ffi.cpp -o /tmp/13b_cpp
// Executar: /tmp/13b_cpp
#include <cassert>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace {

// Recurso com dono unico: a fronteira nunca expoe o objeto, so o handle.
struct Armazem {
    std::vector<std::string> itens;
    std::size_t teto;

    explicit Armazem(std::size_t teto_por_chamada) : teto(teto_por_chamada) {}

    int guardar(const char* texto, std::size_t n)
    {
        if (texto == nullptr || n == 0 || n > teto) {
            return -1; // recusa na fronteira, antes de qualquer alocacao
        }
        itens.emplace_back(texto, n);
        return 0;
    }

    int ler(std::size_t indice, char* saida, std::size_t cap) const
    {
        if (indice >= itens.size() || saida == nullptr || cap == 0) {
            return -1;
        }
        const std::string& item = itens[indice];
        if (item.size() + 1 > cap) {
            return -2; // nao cabe: erro visivel, nunca truncamento calado
        }
        std::memcpy(saida, item.data(), item.size());
        saida[item.size()] = '\0';
        return 0;
    }
};

using Handle = void*;

} // namespace

// ---- Fronteira extern "C": noexcept em tudo, erro em codigo, dono no handle.
extern "C" {

Handle armazem_criar(std::size_t teto) noexcept
{
    try {
        return new (std::nothrow) Armazem(teto == 0 ? 1024 : teto);
    } catch (...) {
        return nullptr; // excecao nunca atravessa: vira nulo
    }
}

int armazem_guardar(Handle h, const char* texto, std::size_t n) noexcept
{
    if (h == nullptr) {
        return -1;
    }
    try {
        return static_cast<Armazem*>(h)->guardar(texto, n);
    } catch (...) {
        return -3; // bad_alloc e afins viram codigo, nao unwind through C
    }
}

int armazem_ler(Handle h, std::size_t indice, char* saida, std::size_t cap) noexcept
{
    if (h == nullptr) {
        return -1;
    }
    try {
        return static_cast<Armazem*>(h)->ler(indice, saida, cap);
    } catch (...) {
        return -3;
    }
}

void armazem_destruir(Handle h) noexcept
{
    delete static_cast<Armazem*>(h); // delete em nulo e seguro; dono unico
}

} // extern "C"

int main()
{
    // O cliente desta main e C puro em espirito: so usa a fronteira C.
    Handle h = armazem_criar(16);
    assert(h != nullptr);

    assert(armazem_guardar(h, "ola", 3) == 0);
    assert(armazem_guardar(h, nullptr, 3) == -1); // nulo recusado
    assert(armazem_guardar(h, "grande demais para o teto", 25) == -1); // teto
    assert(armazem_guardar(nullptr, "x", 1) == -1); // handle nulo
    std::cout << "fronteira: nulo, teto e handle invalido recusados\n";

    char buf[16];
    assert(armazem_ler(h, 0, buf, sizeof buf) == 0 && std::string(buf) == "ola");
    char pequeno[2];
    assert(armazem_ler(h, 0, pequeno, sizeof pequeno) == -2); // nao cabe: erro
    assert(armazem_ler(h, 9, buf, sizeof buf) == -1); // indice fora
    assert(armazem_ler(h, 0, nullptr, 4) == -1); // saida nula
    std::cout << "leitura: ok, curto, indice e saida cobertos por codigo\n";

    // Dono unico: a fronteira recebe o objeto de volta uma unica vez.
    // Disciplina do cliente: zerar o handle apos destruir — a fronteira
    // recusa nulo, mas NAO rastreia destruicao: usar o valor pendurado
    // depois de destruir seria use-after-free, e o contrato proibe.
    armazem_destruir(h);
    h = nullptr;
    assert(armazem_ler(h, 0, buf, sizeof buf) == -1); // nulo: recusado
    std::cout << "pos-destruir: nulo recusado; pendurado seria UAF (contrato)\n";
    std::cout << "13b ok: excecao nao atravessa, dono e unico, abuso vira codigo\n";
    return 0;
}
