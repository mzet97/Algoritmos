// Aula 13a — seguranca de memoria em C++: reproduzir a falha sob ASan,
// diagnosticar e rodar a versao corrigida. Modos de falha so executam
// quando pedidos na linha de comando (processo separado, crash esperado).
#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

// ---------- versoes corrigidas (o caminho padrao) ----------
static int seguro() {
    // 1) Limite: at() lanca em vez de ler fora; span-like por tamanho explicito.
    std::vector<int> v{10, 20, 30};
    bool lancou = false;
    try {
        (void)v.at(3);
    } catch (const std::out_of_range&) {
        lancou = true;
    }
    assert(lancou);

    // 2) Ownership: unique_ptr libera sozinho, inclusive em retorno antecipado.
    {
        auto p = std::make_unique<std::string>("dono unico");
        assert(*p == "dono unico");
    } // destruido aqui, sem free manual

    // 3) Conta de alocacao verificada ANTES de multiplicar (o overflow de
    //    tamanho da aula 07 de C, na versao C++: reserve() com n hostil).
    std::size_t n = SIZE_MAX / sizeof(int) + 1; // n * 4 transbordaria
    bool recusou = false;
    if (n > SIZE_MAX / sizeof(int)) recusou = true; // guarda: nao tenta alocar
    assert(recusou);

    // 4) Copia com teto: snprintf declara o destino e trunca com aviso, nunca
    //    escreve alem. Entrada maior que o buffer vira erro visivel.
    char dest[8];
    std::string entrada = "entrada-hostil";
    entrada += "-muito-longa"; // tamanho so conhecido em execucao
    int escritos = std::snprintf(dest, sizeof dest, "%s", entrada.c_str());
    assert(escritos >= static_cast<int>(sizeof dest)); // sinal de truncamento
    assert(std::strlen(dest) < sizeof dest);

    std::cout << "seguro ok: at() lanca, unique_ptr libera, conta verificada, copia com teto\n";
    return 0;
}

// ---------- falhas reproduziveis (rodar sob ASan, uma por processo) ----------
static int falha_overflow() {
    auto* v = new std::vector<int>(4, 0);
    volatile int* dados = v->data();
    dados[4] = 123; // heap-buffer-overflow: ASan deve apontar aqui
    delete v;
    return 0;
}

static int falha_uaf() {
    auto* p = new std::string("curto");
    delete p;
    volatile std::size_t n = p->size(); // use-after-free
    return static_cast<int>(n);
}

static int falha_double_free() {
    auto* p = new int(7);
    delete p;
    delete p; // double free
    return 0;
}

int main(int argc, char** argv) {
    if (argc > 1) {
        std::string modo = argv[1];
        if (modo == "overflow") return falha_overflow();
        if (modo == "uaf") return falha_uaf();
        if (modo == "doublefree") return falha_double_free();
        std::cerr << "uso: " << argv[0] << " [overflow|uaf|doublefree]\n";
        return 2;
    }
    return seguro();
}
