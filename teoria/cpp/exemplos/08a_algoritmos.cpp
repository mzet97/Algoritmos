// Aula 08a — algoritmos em C++: implementar a mao, confrontar com a STL.
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

static std::mt19937 rng(42);

// Busca binaria manual: invariante [ini,fim) como na trilha de C (13a/13b).
static long busca_binaria(const std::vector<int>& v, int alvo) {
    std::size_t ini = 0, fim = v.size();
    while (ini < fim) {
        std::size_t meio = ini + (fim - ini) / 2;
        if (v[meio] == alvo) return static_cast<long>(meio);
        if (alvo < v[meio]) fim = meio; else ini = meio + 1;
    }
    return -1;
}

// Ordenacao por insercao (quadratica) e mergesort estavel (n log n), a mao.
static void insercao(std::vector<int>& v) {
    for (std::size_t i = 1; i < v.size(); i++) {
        int x = v[i];
        std::size_t j = i;
        while (j > 0 && v[j - 1] > x) { v[j] = v[j - 1]; j--; }
        v[j] = x;
    }
}

static void mergesort(std::vector<int>& v, std::vector<int>& aux,
                      std::size_t ini, std::size_t fim) {
    if (fim - ini < 2) return;
    std::size_t meio = ini + (fim - ini) / 2;
    mergesort(v, aux, ini, meio);
    mergesort(v, aux, meio, fim);
    std::merge(v.begin() + static_cast<long>(ini), v.begin() + static_cast<long>(meio),
               v.begin() + static_cast<long>(meio), v.begin() + static_cast<long>(fim),
               aux.begin() + static_cast<long>(ini));
    std::copy(aux.begin() + static_cast<long>(ini), aux.begin() + static_cast<long>(fim),
              v.begin() + static_cast<long>(ini));
}

int main() {
    // Propriedade: minhas implementacoes == STL como oraculo, 500 casos.
    for (int t = 0; t < 500; t++) {
        std::size_t n = rng() % 80;
        std::vector<int> base(n);
        for (auto& x : base) x = static_cast<int>(rng() % 61) - 30;
        std::vector<int> a = base, b = base, c = base;
        insercao(a);
        std::vector<int> aux(b.size());
        mergesort(b, aux, 0, b.size());
        std::sort(c.begin(), c.end());
        assert(a == c && b == c);
        for (int x : c) assert(busca_binaria(c, x) >= 0);
        assert(busca_binaria(c, 1000) == -1 && busca_binaria(c, -1000) == -1);
        // Oraculo da busca: std::binary_search concorda em tudo.
        for (int alvo = -35; alvo <= 35; alvo++)
            assert((busca_binaria(c, alvo) >= 0) == std::binary_search(c.begin(), c.end(), alvo));
    }
    std::cout << "propriedade ok: insercao==mergesort==std::sort, busca==binary_search\n";

    // Medicao em tres tamanhos: a assintotica aparece ou a aula esta errada.
    for (std::size_t n : {2000u, 8000u, 32000u}) {
        std::vector<int> base(n);
        for (auto& x : base) x = static_cast<int>(rng());
        auto mede = [&](auto fn) {
            auto t0 = std::chrono::steady_clock::now();
            fn();
            return std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
        };
        std::vector<int> a = base, b = base, c = base;
        double t_ins = mede([&] { insercao(a); });
        std::vector<int> aux(b.size());
        double t_mer = mede([&] { mergesort(b, aux, 0, b.size()); });
        double t_std = mede([&] { std::sort(c.begin(), c.end()); });
        assert(a == c && b == c);
        std::cout << "n=" << n << " insercao=" << t_ins << "s mergesort=" << t_mer
                  << "s std::sort=" << t_std << "s\n";
        if (n == 32000u) {
            // Quadruplicar n multiplica insercao por ~16, mergesort por ~4-5.
            assert(t_ins > t_mer * 3);
        }
    }
    std::cout << "08a ok: duas solucoes medidas, assintotica confirmada contra a STL\n";
    return 0;
}
