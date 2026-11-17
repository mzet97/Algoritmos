#include "../bib1.hpp"
#include <algorithm>

std::string reverte(const std::string &L) {
    std::string result(L.rbegin(), L.rend());
    return result;
}

int main() {
    while (true) {
        try {
            std::string L = entrada();
            if (L == "@entradagrande") {
                L.clear();
                L.reserve(1000000);
                for (int i = 0; i < 500000; i++) {
                    L += "ab";
                }
            }
            std::cout << reverte(L) << "\n";
        } catch (...) {
            break;
        }
    }
    return 0;
}
