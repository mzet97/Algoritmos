#include "../bib1.hpp"
#include <vector>
#include <algorithm>

std::vector<int> soma_lista(const std::vector<int>& x, const std::vector<int>& y) {
    std::vector<int> resultado;
    int carry = 0;
    size_t len = std::max(x.size(), y.size());
    for (size_t i = 0; i < len; i++) {
        int a = i < x.size() ? x[i] : 0;
        int b = i < y.size() ? y[i] : 0;
        int s = a + b + carry;
        resultado.push_back(s % 10);
        carry = s / 10;
    }
    if (carry) resultado.push_back(carry);
    return resultado;
}

int main() {
    while (true) {
        try {
            std::string xstr = entrada();
            std::string ystr;
            if (xstr != "@entradagrande") {
                ystr = entrada();
            } else {
                xstr = std::string(1000000, '9');
                ystr = std::string(1000000, '9');
            }
            std::vector<int> xl, yl;
            for (auto it = xstr.rbegin(); it != xstr.rend(); ++it) xl.push_back(*it - '0');
            for (auto it = ystr.rbegin(); it != ystr.rend(); ++it) yl.push_back(*it - '0');
            auto r = soma_lista(xl, yl);
            std::reverse(r.begin(), r.end());
            for (int d : r) std::cout << d;
            std::cout << "\n";
        } catch (...) { break; }
    }
}
