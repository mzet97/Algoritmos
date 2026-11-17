#include "../bib1.hpp"
#include <vector>

int acima_media(const std::vector<int>& l) {
    if (l.empty()) return 0;
    long long soma = 0;
    for (int v : l) soma += v;
    double media = (double)soma / l.size();
    int count = 0;
    for (int v : l) if (v > media) count++;
    return count;
}

int main() {
    while (true) {
        try {
            std::string sn = entrada();
            if (sn == "@entradagrande") {
                std::vector<int> l(1000000);
                for (int i = 0; i < 1000000; i++) l[i] = i;
                std::cout << acima_media(l) << "\n";
            } else {
                int n = std::stoi(sn);
                std::vector<int> l(n);
                for (int i = 0; i < n; i++) l[i] = std::stoi(entrada());
                std::cout << acima_media(l) << "\n";
            }
        } catch (...) { break; }
    }
}
