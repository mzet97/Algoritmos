#include "../bib1.hpp"
#include <vector>

int main() {
    while (true) {
        try {
            int s = std::stoi(entrada());
            int notas[] = {200, 100, 50, 20, 10, 5};
            std::vector<std::pair<int,int>> resultado;
            int restante = s;
            for (int n : notas) {
                if (restante >= n) {
                    int q = restante / n;
                    resultado.push_back({q, n});
                    restante -= q * n;
                }
            }
            std::cout << "[";
            for (size_t i = 0; i < resultado.size(); i++) {
                if (i > 0) std::cout << ", ";
                std::cout << "(" << resultado[i].first << ", " << resultado[i].second << ")";
            }
            if (restante != 0) std::cout << "]";
            std::cout << "]\n";
        } catch (...) { break; }
    }
}
