// Aula 30b — modelo GPU em C++: referencia CPU correta, conta da viagem
// (host->device + kernel + device->host) e o limiar onde a GPU perderia.
// Sem placa neste laboratorio: o modelo decide; a placa confirmaria.
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

static std::uint32_t estado = 3;
static std::uint32_t prox() {
    estado = estado * 1664525u + 1013904223u;
    return estado >> 8;
}

// SAXPY: y = a*x + y (o kernel classico de largura de banda).
static void saxpy_cpu(const std::vector<float>& x, std::vector<float>& y, float a) {
    for (std::size_t i = 0; i < x.size(); i++) y[i] = a * x[i] + y[i];
}

struct ModeloGpu {
    double banda_pcie_GBs;   // host <-> device
    double banda_gpu_GBs;    // memoria do device
    double latencia_s;       // lancamento + sincronizacao por chamada
};

static double tempo_gpu(std::size_t n, const ModeloGpu& m) {
    double bytes = static_cast<double>(n) * 4.0;
    double viagem = 2.0 * bytes / (m.banda_pcie_GBs * 1e9);  // ida e volta de y (x fica)
    double kernel = 3.0 * bytes / (m.banda_gpu_GBs * 1e9);   // le x, le y, escreve y
    return m.latencia_s + viagem + kernel;
}

int main() {
    // Corretude primeiro: a referencia CPU e o oraculo de qualquer GPU.
    for (std::size_t n : {0u, 1u, 7u, 1000u}) {
        std::vector<float> x(n), y(n);
        for (std::size_t i = 0; i < n; i++) {
            x[i] = static_cast<float>(prox() % 1000) / 1000.0f;
            y[i] = static_cast<float>(prox() % 1000) / 1000.0f;
        }
        std::vector<float> esperado = y;
        saxpy_cpu(x, y, 2.5f);
        for (std::size_t i = 0; i < n; i++) esperado[i] = 2.5f * x[i] + esperado[i];
        assert(y == esperado);
    }
    std::cout << "referencia ok (inclui n=0 e n=1)\n";

    // Tempo CPU medido num tamanho grande (a banda manda, como na 30a de C).
    std::size_t grande = 20'000'000;
    std::vector<float> x(grande), y(grande);
    for (std::size_t i = 0; i < grande; i++) {
        x[i] = static_cast<float>(prox() % 1000) / 1000.0f;
        y[i] = static_cast<float>(prox() % 1000) / 1000.0f;
    }
    auto t0 = std::chrono::steady_clock::now();
    saxpy_cpu(x, y, 1.5f);
    double t_cpu = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
    double banda_cpu = 3.0 * static_cast<double>(grande) * 4.0 / t_cpu / 1e9;
    std::cout << "cpu: saxpy 20M em " << t_cpu << "s (banda efetiva " << banda_cpu << " GB/s)\n";

    // Modelo com numeros de placa PCIe tipica (declarados, nao medidos aqui).
    ModeloGpu gpu{25.0, 900.0, 10e-6};
    for (std::size_t n : {10'000u, 1'000'000u, 20'000'000u, 200'000'000u}) {
        double tg = tempo_gpu(n, gpu);
        double tcpu_est = 3.0 * static_cast<double>(n) * 4.0 / (banda_cpu * 1e9);
        std::cout << "n=" << n << " gpu_modelo=" << tg << "s cpu_est=" << tcpu_est
                  << "s veredito=" << (tg < tcpu_est ? "GPU venceria" : "CPU vence") << "\n";
    }
    // O limiar existe e e alcancavel: para n pequeno a latencia+viagem dominam.
    // (Comparar com banda medida so faz sentido fora de sanitizer.)
#if !defined(__SANITIZE_ADDRESS__) && !defined(__SANITIZE_THREAD__)
    assert(tempo_gpu(10'000, gpu) > 3.0 * 10'000 * 4.0 / (banda_cpu * 1e9));
    assert(tempo_gpu(200'000'000, gpu) < 3.0 * 200'000'000 * 4.0 / (banda_cpu * 1e9));
#endif
    std::cout << "30b ok: referencia correta, modelo com limiar e veredito por tamanho\n";
    return 0;
}
