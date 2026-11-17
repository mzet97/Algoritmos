#include "../bib1.hpp"

int raiz(long long n, long long inf, long long sup) {
    while (sup - inf > 1) {
        long long mid = (inf + sup) / 2;
        if (mid * mid <= n) inf = mid; else sup = mid;
    }
    return (int)inf;
}

int main() {
    while (true) {
        try { std::cout << raiz(std::stoll(entrada()), 0, 0) + 1 << "\n"; }
        catch (...) { break; }
    }
}
