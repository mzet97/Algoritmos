#include "../bib1.hpp"
#include <vector>

double imposto(const std::vector<int>& l) {
    double total = 0.0;
    for (int s : l) {
        if (s <= 3000) continue;
        else if (s <= 8000) total += (s - 3000) * 0.15;
        else total += 5000 * 0.15 + (s - 8000) * 0.27;
    }
    return total;
}

int main() {
    while (true) {
        try {
            int n = std::stoi(entrada());
            std::vector<int> l(n);
            for (int i = 0; i < n; i++) l[i] = std::stoi(entrada());
            std::cout << python_float(imposto(l)) << "\n";
        } catch (...) { break; }
    }
}
