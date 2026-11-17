#include "../bib1.hpp"
#include <vector>

int main() {
    while (true) {
        try {
            int n = std::stoi(entrada());
            std::vector<long long> fibs(n);
            long long a = 1, b = 1;
            for (int i = 0; i < n; i++) { fibs[i] = a; long long t = a+b; a = b; b = t; }
            for (int k = 1; k < n; k++) {
                for (int i = 0; i < k; i++) { if (i > 0) std::cout << " "; std::cout << fibs[i]; }
                std::cout << "\n";
            }
            for (int i = 0; i < n; i++) { if (i > 0) std::cout << " "; std::cout << fibs[i]; }
            std::cout << "\n";
            for (int k = n-1; k >= 1; k--) {
                for (int i = 0; i < k; i++) { if (i > 0) std::cout << " "; std::cout << fibs[i]; }
                std::cout << "\n";
            }
        } catch (...) { break; }
    }
}
