// Aula 30a — HPC multicore em C++: baseline sequencial, particao por
// linhas com std::thread, corretude antes de cronometrar e a conta de
// quando paralelizar perde (trabalho pequeno demais para o custo da thread).
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>

static std::uint32_t estado = 11;
static std::uint32_t prox() {
    estado = estado * 1664525u + 1013904223u;
    return estado >> 8;
}

using Mat = std::vector<double>;
using Vec = std::vector<double>;

// y = A * x, A row-major n x n.
static void matvec_seq(const Mat& A, const Vec& x, Vec& y, std::size_t n) {
    for (std::size_t i = 0; i < n; i++) {
        double s = 0;
        for (std::size_t j = 0; j < n; j++) s += A[i * n + j] * x[j];
        y[i] = s;
    }
}

static void matvec_par(const Mat& A, const Vec& x, Vec& y, std::size_t n, unsigned threads) {
    std::vector<std::thread> ts;
    auto trabalho = [&](std::size_t ini, std::size_t fim) {
        for (std::size_t i = ini; i < fim; i++) {
            double s = 0;
            for (std::size_t j = 0; j < n; j++) s += A[i * n + j] * x[j];
            y[i] = s;
        }
    };
    std::size_t bloco = (n + threads - 1) / threads;
    for (unsigned t = 0; t < threads; t++) {
        std::size_t ini = t * bloco, fim = std::min(n, ini + bloco);
        if (ini < fim) ts.emplace_back(trabalho, ini, fim);
    }
    for (auto& th : ts) th.join();
}

static double mede(auto&& fn) {
    auto t0 = std::chrono::steady_clock::now();
    fn();
    return std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
}

int main() {
    unsigned hw = std::thread::hardware_concurrency();
    std::cout << "hardware_concurrency=" << hw << "\n";
    assert(hw >= 1);

    for (std::size_t n : {400u, 1200u}) {
        Mat A(n * n);
        Vec x(n), y1(n), y2(n);
        for (auto& v : A) v = static_cast<double>(prox() % 1000) / 1000.0;
        for (auto& v : x) v = static_cast<double>(prox() % 1000) / 1000.0;

        matvec_seq(A, x, y1, n);
        matvec_par(A, x, y2, n, hw);
        double maior = 0;
        for (std::size_t i = 0; i < n; i++) maior = std::max(maior, std::abs(y1[i] - y2[i]));
        // Mesma ordem de soma dentro da linha: resultados identicos.
        assert(maior == 0.0);

        double t_seq = mede([&] { matvec_seq(A, x, y1, n); });
        double t_par = mede([&] { matvec_par(A, x, y2, n, hw); });
        std::cout << "n=" << n << " seq=" << t_seq << "s par(" << hw << ")=" << t_par
                  << "s razao=" << (t_seq / t_par) << "x\n";
        // Sob sanitizer o custo relativo muda; speedup so se afirma sem ele.
#if !defined(__SANITIZE_ADDRESS__) && !defined(__SANITIZE_THREAD__)
        if (n == 1200u) assert(t_par < t_seq); // trabalho basta: paralelo vence
#endif
    }

    // Trabalho pequeno: criar threads custa mais que o proprio calculo.
    {
        std::size_t n = 40;
        Mat A(n * n, 1.0);
        Vec x(n, 1.0), y(n);
        double t_seq = 1e9, t_par = 1e9;
        for (int r = 0; r < 5; r++) t_seq = std::min(t_seq, mede([&] { matvec_seq(A, x, y, n); }));
        for (int r = 0; r < 5; r++) t_par = std::min(t_par, mede([&] { matvec_par(A, x, y, n, hw); }));
        std::cout << "n=40 seq=" << t_seq << "s par=" << t_par
                  << "s (paralelo perde: overhead domina)\n";
        assert(t_par > t_seq);
    }
    std::cout << "30a ok: corretude identica, ganho medido e caso em que paralelizar perde\n";
    return 0;
}
