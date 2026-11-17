#include "../bib1.hpp"

int contar(const std::string &L, char c) {
    int count = 0;
    for (char ch : L) {
        if (ch == c) count++;
    }
    return count;
}

int main() {
    while (true) {
        try {
            std::string L = entrada();
            std::string cs = entrada();
            char c = cs[0];
            std::cout << contar(L, c) << "\n";
        } catch (...) {
            break;
        }
    }
    return 0;
}
