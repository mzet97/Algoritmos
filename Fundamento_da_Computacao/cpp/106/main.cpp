#include "../bib1.hpp"
#include <vector>
#include <algorithm>

std::vector<int> subtrai_lista(const std::vector<int>& x, const std::vector<int>& y) {
    std::vector<int> resultado;
    int borrow = 0;
    for (size_t i = 0; i < x.size(); i++) {
        int a = x[i];
        int b = i < y.size() ? y[i] : 0;
        a -= borrow;
        if (a < b) { a += 10; borrow = 1; } else { borrow = 0; }
        resultado.push_back(a - b);
    }
    while (resultado.size() > 1 && resultado.back() == 0) resultado.pop_back();
    return resultado;
}

int main() {
    while (true) {
        try {
            std::string xstr = entrada();
            std::string ystr;
            if (xstr == "@entradagrande1") {
                xstr = std::string(1000000, '1');
                ystr = std::string(999999, '9');
            } else if (xstr == "@entradagrande2") {
                xstr = std::string(1000000, '1');
                ystr = std::string(1000000, '1');
            } else {
                ystr = entrada();
            }
            std::vector<int> xl, yl;
            for (auto it = xstr.rbegin(); it != xstr.rend(); ++it) xl.push_back(*it - '0');
            for (auto it = ystr.rbegin(); it != ystr.rend(); ++it) yl.push_back(*it - '0');
            auto r = subtrai_lista(xl, yl);
            std::reverse(r.begin(), r.end());
            for (int d : r) std::cout << d;
            std::cout << "\n";
        } catch (...) { break; }
    }
}
