#include <atomic>
#include <cassert>
#include <chrono>
#include <csignal>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

// Flag async-signal-safe: o handler so escreve, o laco so le.
volatile std::sig_atomic_t parar = 0;

extern "C" void ao_sinal(int) {
    parar = 1; // unico gesto seguro aqui: escrever o flag
}

static double agora_s() {
    return std::chrono::duration<double>(
               std::chrono::steady_clock::now().time_since_epoch())
        .count();
}

int main() {
    std::signal(SIGINT, ao_sinal);
    std::signal(SIGTERM, ao_sinal);

    // 1. Contador sob mutex: invariante verificada, sem TSan reclamando.
    std::mutex mu;
    long total = 0;
    const int threads = 8, repeticoes = 100000;
    std::vector<std::thread> ts;
    for (int t = 0; t < threads; ++t) {
        ts.emplace_back([&] {
            for (int i = 0; i < repeticoes; ++i) {
                std::lock_guard<std::mutex> g(mu);
                ++total;
            }
        });
    }
    for (auto &t : ts) {
        t.join();
    }
    ts.clear();
    assert(total == (long)threads * repeticoes);
    std::cout << "mutex ok: total = " << total << "\n";

    // 2. Contador atomico: mesma conta, sem lock.
    std::atomic<long> atomico{0};
    for (int t = 0; t < threads; ++t) {
        ts.emplace_back([&] {
            for (int i = 0; i < repeticoes; ++i) {
                atomico.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }
    for (auto &t : ts) {
        t.join();
    }
    ts.clear();
    assert(atomico.load() == (long)threads * repeticoes);
    std::cout << "atomico ok: total = " << atomico.load() << "\n";

    // 3. Escala 1 x N: soma particionada, invariante fechada.
    const long n = 2000000L;
    const int ncpu = (int)std::max(1u, std::thread::hardware_concurrency());
    auto somar_faixa = [](long ini, long fim) {
        long s = 0;
        for (long i = ini; i < fim; ++i) {
            s += i;
        }
        return s;
    };
    const double t0 = agora_s();
    const long seq = somar_faixa(0, n);
    const double t_seq = agora_s() - t0;

    const double t1 = agora_s();
    std::vector<std::thread> ws;
    std::vector<long> parciais((std::size_t)ncpu, 0);
    for (int w = 0; w < ncpu; ++w) {
        ws.emplace_back([&, w] {
            parciais[(std::size_t)w] = somar_faixa(n * w / ncpu, n * (w + 1) / ncpu);
        });
    }
    for (auto &t : ws) {
        t.join();
    }
    long par = 0;
    for (long p : parciais) {
        par += p;
    }
    const double t_par = agora_s() - t1;
    assert(seq == par && seq == n * (n - 1) / 2);
    std::cout << "seq=" << t_seq << "s par(" << ncpu << ")=" << t_par
              << "s razao=" << t_seq / t_par << "x\n";

    // 4. Sinal: o laco respeita o flag sem fazer nada proibido no handler.
    for (int i = 0; i < 3 && !parar; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::cout << "sinal respeitado (rode com timeout -s TERM para testar)\n";
    return 0;
}
