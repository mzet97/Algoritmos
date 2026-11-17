#include "../bib1.hpp"

long long fatorial(int n) {
    long long r = 1;
    for (int i = 2; i <= n; i++) r *= i;
    return r;
}

int main() {
    while (true) {
        try { std::cout << fatorial(std::stoi(entrada())) << "\n"; }
        catch (...) { break; }
    }
}
