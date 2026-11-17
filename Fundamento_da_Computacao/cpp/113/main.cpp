#include "../bib1.hpp"
#include <vector>
#include <tuple>

std::tuple<int,int> conta_num(const std::vector<int>& lista) {
    int qp = 0, qn = 0;
    for (int x : lista) { if (x > 0) qp++; if (x < 0) qn++; }
    return {qp, qn};
}

int main() {
    while (true) {
        try {
            int n = std::stoi(entrada());
            std::vector<int> lista(n);
            for (int i = 0; i < n; i++) lista[i] = std::stoi(entrada());
            auto [qp, qn] = conta_num(lista);
            std::cout << "(" << qp << ", " << qn << ")\n";
        } catch (...) { break; }
    }
}
