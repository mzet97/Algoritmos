#include "../bib1.hpp"
#include <cmath>
#include <iomanip>
#include <sstream>

std::string cientifico(double n) {
    if (n == 0.0) return "0.0x10^0";
    std::string sinal;
    if (n < 0) { sinal = "-"; n = -n; }
    int exp = 0;
    while (n >= 10) { n /= 10; exp++; }
    while (n < 1) { n *= 10; exp--; }
    std::ostringstream oss;
    oss << sinal << std::fixed << std::setprecision(1) << n << "x10^" << exp;
    return oss.str();
}

int main() {
    while (true) {
        try { std::cout << cientifico(std::stod(entrada())) << "\n"; }
        catch (...) { break; }
    }
}
