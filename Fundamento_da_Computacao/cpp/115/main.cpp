#include "../bib1.hpp"
#include <cmath>
#include <optional>
#include <tuple>

auto raiz(double a, double b, double c) -> std::optional<std::tuple<double, std::optional<double>>> {
    double delta = b*b - 4*a*c;
    if (delta < 0) return std::nullopt;
    double r1 = (-b + std::sqrt(delta)) / (2*a);
    if (delta == 0) return {{r1, std::nullopt}};
    double r2 = (-b - std::sqrt(delta)) / (2*a);
    return {{r1, r2}};
}

void print_val(double v) {
    if (v == (long long)v && std::fabs(v) < 1e15) std::cout << (long long)v;
    else std::cout << python_float(v);
}

int main() {
    while (true) {
        try {
            double a = std::stod(entrada()), b = std::stod(entrada()), c = std::stod(entrada());
            auto res = raiz(a, b, c);
            if (!res) { std::cout << "None\n"; continue; }
            auto [r1, r2] = *res;
            if (!r2) { std::cout << "("; print_val(r1); std::cout << ", None)\n"; }
            else { std::cout << "("; print_val(r1); std::cout << ", "; print_val(*r2); std::cout << ")\n"; }
        } catch (...) { break; }
    }
}
