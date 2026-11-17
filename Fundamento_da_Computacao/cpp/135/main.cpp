#include "../bib1.hpp"
#include <vector>

int main() {
    std::vector<int> resultados;
    for (int n = 1000; n < 10000; n++) {
        int esq = n / 100, dir = n % 100;
        if ((esq + dir) * (esq + dir) == n) resultados.push_back(n);
    }
    std::cout << "[";
    for (size_t i = 0; i < resultados.size(); i++) {
        if (i > 0) std::cout << ", ";
        std::cout << resultados[i];
    }
    std::cout << "]\n";
}
