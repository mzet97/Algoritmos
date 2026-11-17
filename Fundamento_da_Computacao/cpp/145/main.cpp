#include "../bib1.hpp"

double serie(int N) {
    double sum = 0.0;
    for (int i = 1; i <= N; i++) {
        sum += static_cast<double>(i) / (N - i + 1);
    }
    return sum;
}

int main() {
    while (true) {
        try {
            int N = std::stoi(entrada());
            std::cout << python_float(serie(N)) << "\n";
        } catch (...) {
            break;
        }
    }
    return 0;
}
