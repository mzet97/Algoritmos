#include "../bib1.hpp"

long long raiz(long long n) {
    long long i = 0;
    while ((i + 1) * (i + 1) <= n) {
        i++;
    }
    return i;
}

int main() {
    while (true) {
        try {
            long long n = std::stoll(entrada());
            std::cout << raiz(n) << "\n";
        } catch (...) {
            break;
        }
    }
    return 0;
}
