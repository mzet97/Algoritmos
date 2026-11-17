#include "../bib1.hpp"

bool perfeito(int n) {
    if (n < 2) return false;
    int soma = 1;
    for (int i = 2; (long long)i * i <= n; i++) {
        if (n % i == 0) { soma += i; if (i != n/i) soma += n/i; }
    }
    return soma == n;
}

int main() {
    while (true) {
        try { std::cout << (perfeito(std::stoi(entrada())) ? "True" : "False") << "\n"; }
        catch (...) { break; }
    }
}
