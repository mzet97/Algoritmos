#include "../bib1.hpp"
#include <cmath>
#include <tuple>

auto tv(int d, int pl, int ph) {
    double diag = d * 2.54;
    double ratio = (double)pl / ph;
    double h = diag / std::sqrt(1 + ratio*ratio);
    double w = h * ratio;
    w = std::round(w*10)/10;
    h = std::round(h*10)/10;
    return std::make_pair(w, h);
}

void pf(double v) {
    if (v == (long long)v && std::fabs(v) < 1e15) std::cout << (long long)v;
    else std::cout << python_float(v);
}

int main() {
    while (true) {
        try {
            int d = std::stoi(entrada()), pl = std::stoi(entrada()), ph = std::stoi(entrada());
            auto [w, h] = tv(d, pl, ph);
            std::cout << "("; pf(w); std::cout << ", "; pf(h); std::cout << ")\n";
        } catch (...) { break; }
    }
}
