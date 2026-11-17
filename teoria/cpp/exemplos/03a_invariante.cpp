#include <cassert>
#include <chrono>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

// Contrato: a e b em unsigned int; devolve mdc(a, b), com mdc(0,0) == 0.
// Invariante: mdc(a, b) do par atual == mdc das entradas originais.
unsigned int mdc(unsigned int a, unsigned int b) {
    while (b != 0u) {
        const unsigned int resto = a % b;
        a = b;
        b = resto;
    }
    return a;
}

// Versao por subtracao, apenas para entradas positivas: contrato menor.
unsigned int mdc_sub(unsigned int a, unsigned int b) {
    assert(a > 0u && b > 0u);
    while (a != b) {
        if (a > b) {
            a -= b;
        } else {
            b -= a;
        }
    }
    return a;
}

long long medir_us(auto funcao, unsigned int a, unsigned int b, int repeticoes) {
    const auto inicio = std::chrono::steady_clock::now();
    for (int i = 0; i < repeticoes; ++i) {
        volatile unsigned int r = funcao(a, b);
        (void)r;
    }
    const auto fim = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(fim - inicio).count();
}

int main() {
    assert(mdc(54u, 24u) == 6u);
    assert(mdc(0u, 24u) == 24u);
    assert(mdc(24u, 0u) == 24u);
    assert(mdc(0u, 0u) == 0u);
    assert(mdc_sub(54u, 24u) == 6u);

    // Teste de propriedade: oraculo independente (std::gcd) com semente fixa.
    std::mt19937 rng(42);  // semente fixa: a falha e reproduzivel
    std::uniform_int_distribution<unsigned int> dist(0u, 100000u);
    for (int i = 0; i < 2000; ++i) {
        const unsigned int a = dist(rng);
        const unsigned int b = dist(rng);
        assert(mdc(a, b) == std::gcd(a, b));
        if (a > 0u && b > 0u) {
            assert(mdc_sub(a, b) == std::gcd(a, b));
        }
    }

    std::cout << "mdc(54, 24) = " << mdc(54u, 24u) << "\n";
    const int rep = 20000;
    std::cout << "modulo (987,986): " << medir_us(mdc, 987u, 986u, rep) << " us\n";
    std::cout << "subtracao (987,986): " << medir_us(mdc_sub, 987u, 986u, rep) << " us\n";
    std::cout << "modulo (987,610): " << medir_us(mdc, 987u, 610u, rep) << " us\n";
    return 0;
}
