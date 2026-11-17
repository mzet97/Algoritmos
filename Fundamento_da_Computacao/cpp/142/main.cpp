#include "../bib1.hpp"
#include <cmath>

char ponto_reta(double a, double b, double x, double y) {
    double diff = y - (a*x + b);
    if (std::fabs(diff) <= 0.00001) return '=';
    return diff > 0 ? '>' : '<';
}

int main() {
    while (true) {
        try {
            double a = std::stod(entrada()), b = std::stod(entrada());
            double x = std::stod(entrada()), y = std::stod(entrada());
            std::cout << ponto_reta(a, b, x, y) << "\n";
        } catch (...) { break; }
    }
}
