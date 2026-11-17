#include "../bib1.hpp"
#include <vector>

double troco(const std::vector<int>& m, const std::vector<int>& q) {
    long long total = 0;
    for (size_t i = 0; i < m.size(); i++) total += (long long)m[i] * q[i];
    return (double)total / 100.0;
}

int main() {
    while (true) {
        try {
            int n = std::stoi(entrada());
            std::vector<int> m(n), q(n);
            for (int i = 0; i < n; i++) m[i] = std::stoi(entrada());
            for (int i = 0; i < n; i++) q[i] = std::stoi(entrada());
            std::cout << python_float(troco(m, q)) << "\n";
        } catch (...) { break; }
    }
}
