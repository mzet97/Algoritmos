#include "../bib1.hpp"

long long fibonacci(int n) {
    if (n <= 0) return 0;
    if (n <= 2) return 1;
    long long a = 1, b = 1;
    for (int i = 2; i < n; i++) { long long t = a+b; a = b; b = t; }
    return b;
}

int main() {
    while (true) {
        try { std::cout << fibonacci(std::stoi(entrada())) << "\n"; }
        catch (...) { break; }
    }
}
