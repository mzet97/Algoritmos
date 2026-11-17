#include "../bib1.hpp"

long long calc_sec(int h1, int m1, int h2, int m2) {
    long long s1 = (long long)h1 * 3600 + (long long)m1 * 60;
    long long s2 = (long long)h2 * 3600 + (long long)m2 * 60;
    long long diff = s2 - s1;
    if (diff < 0) diff = -diff;
    return diff;
}

int main() {
    while (true) {
        try {
            int h1 = std::stoi(entrada());
            int m1 = std::stoi(entrada());
            int h2 = std::stoi(entrada());
            int m2 = std::stoi(entrada());
            std::cout << calc_sec(h1, m1, h2, m2) << "\n";
        } catch (...) {
            break;
        }
    }
    return 0;
}
