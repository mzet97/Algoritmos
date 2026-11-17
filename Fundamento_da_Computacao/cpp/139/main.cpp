#include "../bib1.hpp"
#include <vector>

int main() {
    while (true) {
        try {
            long long a = std::stoll(entrada());
            int q = std::stoi(entrada()), n = std::stoi(entrada());
            std::cout << "[";
            for (int i = 0; i < n; i++) {
                if (i > 0) std::cout << ", ";
                std::cout << a;
                a *= q;
            }
            std::cout << "]\n";
        } catch (...) { break; }
    }
}
