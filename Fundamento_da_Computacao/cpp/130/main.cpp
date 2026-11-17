#include "../bib1.hpp"

bool sub_string(const std::string &L, const std::string &S) {
    return L.find(S) != std::string::npos;
}

int main() {
    while (true) {
        try {
            std::string L = entrada();
            std::string S;
            if (L == "@entradagrande") {
                L.clear();
                L.reserve(4000);
                for (int i = 0; i < 2000; i++) {
                    L += "ab";
                }
                S.reserve(2001);
                for (int i = 0; i < 1000; i++) {
                    S += "ab";
                }
                S += 'c';
            } else {
                S = entrada();
            }
            std::cout << (sub_string(L, S) ? "True" : "False") << "\n";
        } catch (...) {
            break;
        }
    }
    return 0;
}
