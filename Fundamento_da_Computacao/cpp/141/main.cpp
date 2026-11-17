#include "../bib1.hpp"
#include <vector>
#include <limits>
#include <algorithm>

int main() {
    while (true) {
        try {
            int nval = std::stoi(entrada());
            std::vector<int> mat;
            std::vector<double> notas;
            int n;
            if (nval >= 0) {
                n = nval;
                mat.resize(n); notas.resize(n);
                for (int i = 0; i < n; i++) mat[i] = std::stoi(entrada());
                for (int i = 0; i < n; i++) notas[i] = std::stod(entrada());
            } else {
                n = 1000000;
                mat.resize(n); notas.resize(n);
                for (int i = 0; i < n; i++) mat[i] = i;
                for (int i = 0; i < 999998; i++) notas[i] = 2.0;
                notas[999998] = 1.0; notas[999999] = 3.0;
            }
            double maxN = *std::max_element(notas.begin(), notas.end());
            double minN = *std::min_element(notas.begin(), notas.end());
            std::cout << "([";
            bool first = true;
            for (int i = 0; i < n; i++) if (notas[i] == maxN) { if (!first) std::cout << ", "; std::cout << mat[i]; first = false; }
            std::cout << "], [";
            first = true;
            for (int i = 0; i < n; i++) if (notas[i] == minN) { if (!first) std::cout << ", "; std::cout << mat[i]; first = false; }
            std::cout << "])\n";
        } catch (...) { break; }
    }
}
