#include "../bib1.hpp"

long long mdc(long long x, long long y) {
    while (y != 0) {
        long long temp = y;
        y = x % y;
        x = temp;
    }
    return x;
}

long long mmc(long long x, long long y) {
    return (x / mdc(x, y)) * y;
}

long long multiplos(long long a, long long b, long long n) {
    long long lcm = mmc(a, b);
    if (lcm == 0) return 0;
    return (n - 1) / lcm;
}

int main() {
    while (true) {
        try {
            long long a = std::stoll(entrada());
            long long b = std::stoll(entrada());
            long long n = std::stoll(entrada());
            std::cout << multiplos(a, b, n) << "\n";
        } catch (...) {
            break;
        }
    }
    return 0;
}
