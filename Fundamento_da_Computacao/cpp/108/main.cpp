#include "../bib1.hpp"
#include <vector>
#include <algorithm>

std::vector<int> multiplica_lista_lista(const std::vector<int>& x, const std::vector<int>& y) {
    std::vector<int> r(x.size() + y.size(), 0);
    for (size_t i = 0; i < x.size(); i++) {
        int carry = 0;
        for (size_t j = 0; j < y.size(); j++) {
            int p = r[i+j] + x[i]*y[j] + carry;
            r[i+j] = p % 10;
            carry = p / 10;
        }
        size_t k = i + y.size();
        while (carry > 0) { int p = r[k] + carry; r[k] = p % 10; carry = p / 10; k++; }
    }
    while (r.size() > 1 && r.back() == 0) r.pop_back();
    return r;
}

int main() {
    while (true) {
        try {
            std::string xstr = entrada();
            std::string ystr;
            if (xstr != "@entradagrande") { ystr = entrada(); }
            else { xstr = std::string(1000, '9'); ystr = std::string(1000, '9'); }
            std::vector<int> xl, yl;
            for (auto it = xstr.rbegin(); it != xstr.rend(); ++it) xl.push_back(*it - '0');
            for (auto it = ystr.rbegin(); it != ystr.rend(); ++it) yl.push_back(*it - '0');
            auto r = multiplica_lista_lista(xl, yl);
            std::reverse(r.begin(), r.end());
            for (int d : r) std::cout << d;
            std::cout << "\n";
        } catch (...) { break; }
    }
}
