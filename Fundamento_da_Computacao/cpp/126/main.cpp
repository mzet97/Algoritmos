#include "../bib1.hpp"

std::string remove(const std::string &L, char c) {
    std::string result;
    result.reserve(L.size());
    for (char ch : L) {
        if (ch != c) result += ch;
    }
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
                    L += "ca";
                }
            }
            std::string cs = entrada();
            char c = cs[0];
            std::cout << remove(L, c) << "\n";
        } catch (...) {
            break;
        }
    }
    return 0;
}
