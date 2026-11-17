#include "../bib1.hpp"

int conta_unicos(const std::string &P) {
    int freq[256] = {};
    for (unsigned char ch : P) {
        freq[ch]++;
    }
    int count = 0;
    for (unsigned char ch : P) {
        if (freq[ch] == 1) count++;
    }
    return count;
}

int main() {
    while (true) {
        try {
            int N = std::stoi(entrada());
            std::string P;
            if (N >= 0) {
                P = entrada();
            } else {
                P.reserve(50000);
                for (int i = 0; i < 10000; i++) {
                    P += "abcde";
                }
            }
            std::cout << conta_unicos(P) << "\n";
        } catch (...) {
            break;
        }
    }
    return 0;
}
