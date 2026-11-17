#include "../bib1.hpp"
#include <vector>

int main() {
    while (true) {
        try {
            int a = std::stoi(entrada()), r = std::stoi(entrada()), n = std::stoi(entrada());
            std::cout << "[";
            for (int i = 0; i < n; i++) {
                if (i > 0) std::cout << ", ";
                std::cout << a + i * r;
            }
            std::cout << "]\n";
        } catch (...) { break; }
    }
}
