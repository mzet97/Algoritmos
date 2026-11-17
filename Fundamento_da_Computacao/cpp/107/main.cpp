#include "../bib1.hpp"
#include <vector>
#include <algorithm>

std::vector<int> multiplica_lista(const std::vector<int>& x, int y) {
    std::vector<int> resultado;
    int carry = 0;
    for (int d : x) {
        int p = d * y + carry;
        resultado.push_back(p % 10);
        carry = p / 10;
    }
    while (carry > 0) { resultado.push_back(carry % 10); carry /= 10; }
    if (resultado.empty()) resultado.push_back(0);
    return resultado;
}

int main() {
    while (true) {
        try {
            std::string xstr = entrada();
            int y = std::stoi(entrada());
            std::vector<int> xl;
            for (auto it = xstr.rbegin(); it != xstr.rend(); ++it) xl.push_back(*it - '0');
            auto r = multiplica_lista(xl, y);
            std::reverse(r.begin(), r.end());
            for (int d : r) std::cout << d;
            std::cout << "\n";
        } catch (...) { break; }
    }
}
