#include "../bib1.hpp"
#include <cmath>
#include <iomanip>

double pi(int N) {
    double S = 0.0;
    for (int i = 0; i < N; i++) {
        double denom = (2.0 * i + 1.0);
        double term = 1.0 / (denom * denom * denom);
        if (i % 2 == 0) {
            S += term;
        } else {
            S -= term;
        }
    }
    return std::cbrt(32.0 * S);
}

int main() {
    while (true) {
        try {
            int N = std::stoi(entrada());
            std::cout << std::fixed << std::setprecision(10) << pi(N) << "\n";
        } catch (...) {
            break;
        }
    }
    return 0;
}
