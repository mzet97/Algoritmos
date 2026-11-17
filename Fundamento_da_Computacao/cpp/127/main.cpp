#include "../bib1.hpp"

static bool is_vowel(char c) {
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u'
        || c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U';
}

std::string duplica(const std::string &L) {
    std::string result;
    result.reserve(L.size() * 2);
    for (char ch : L) {
        result += ch;
        if (is_vowel(ch)) result += ch;
    }
    return result;
}

int main() {
    while (true) {
        try {
            std::string L = entrada();
            if (L == "@entradagrande") {
                L.assign(500000, 'a');
            }
            std::cout << duplica(L) << "\n";
        } catch (...) {
            break;
        }
    }
    return 0;
}
