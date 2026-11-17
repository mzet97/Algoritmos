#include "../bib1.hpp"
#include <vector>
#include <algorithm>
#include <cstdint>

uint64_t lista_para_int(const std::vector<int>& lst) {
    uint64_t val = 0;
    for (int i = (int)lst.size()-1; i >= 0; i--) val = val*10 + lst[i];
    return val;
}

std::vector<int> int_para_lista(uint64_t val) {
    if (val == 0) return {0};
    std::vector<int> lst;
    while (val > 0) { lst.push_back(val % 10); val /= 10; }
    return lst;
}

int main() {
    while (true) {
        try {
            std::string xstr = entrada();
            std::string ystr = entrada();
            std::vector<int> xl, yl;
            for (auto it = xstr.rbegin(); it != xstr.rend(); ++it) xl.push_back(*it - '0');
            for (auto it = ystr.rbegin(); it != ystr.rend(); ++it) yl.push_back(*it - '0');
            uint64_t dx = lista_para_int(xl), dy = lista_para_int(yl);
            auto q = int_para_lista(dx/dy), rem = int_para_lista(dx%dy);
            std::reverse(q.begin(), q.end());
            std::reverse(rem.begin(), rem.end());
            std::cout << "(";
            for (int d : q) std::cout << d;
            std::cout << ",";
            for (int d : rem) std::cout << d;
            std::cout << ")\n";
        } catch (...) { break; }
    }
}
