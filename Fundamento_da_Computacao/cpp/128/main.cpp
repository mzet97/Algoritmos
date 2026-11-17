#include "../bib1.hpp"

bool eh_palindromo(const std::string &L) {
    // Build string without spaces
    std::string s;
    s.reserve(L.size());
    for (char ch : L) {
        if (ch != ' ') s += ch;
    }
    int n = (int)s.size();
    for (int i = 0; i < n / 2; i++) {
        if (s[i] != s[n - 1 - i]) return false;
    }
    return true;
}

int main() {
    while (true) {
        try {
            std::string L = entrada();
            if (L == "@entradagrande1") {
                L.clear();
                L.reserve(1000001);
                for (int i = 0; i < 500000; i++) {
                    L += "ab";
                }
                L += 'a';
            } else if (L == "@entradagrande2") {
                L.clear();
                L.reserve(1000000);
                for (int i = 0; i < 500000; i++) {
                    L += "ab";
                }
            }
            std::cout << (eh_palindromo(L) ? "True" : "False") << "\n";
        } catch (...) {
            break;
        }
    }
    return 0;
}
