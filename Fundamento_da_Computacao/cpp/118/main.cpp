#include "../bib1.hpp"

long long mdc(long long x, long long y) {
    while (y != 0) {
        long long temp = y;
        y = x % y;
        x = temp;
    }
    return x;
}

int main() {
    while (true) {
        try {
            long long x = std::stoll(entrada());
            long long y = std::stoll(entrada());
            std::cout << mdc(x, y) << "\n";
        } catch (...) {
            break;
        }
    }
    return 0;
}
