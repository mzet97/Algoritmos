#include "../bib1.hpp"

double calc_dist(double v, double a, double t) {
    return v * t + 0.5 * a * t * t;
}

int main() {
    while (true) {
        try {
            double v = std::stod(entrada());
            double a = std::stod(entrada());
            double t = std::stod(entrada());
            std::cout << python_float(calc_dist(v, a, t)) << "\n";
        } catch (...) {
            break;
        }
    }
    return 0;
}
