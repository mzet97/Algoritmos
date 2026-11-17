#include "../bib1.hpp"
#include <cmath>

bool eh_primo(long long x) {
    if (x < 2) return false;
    if (x == 2) return true;
    if (x % 2 == 0) return false;
    for (long long i = 3; i * i <= x; i += 2) {
        if (x % i == 0) return false;
    }
    return true;
}

int main() {
    while (true) {
        try {
            long long x = std::stoll(entrada());
            std::cout << (eh_primo(x) ? "True" : "False") << "\n";
        } catch (...) {
            break;
        }
    }
    return 0;
}
