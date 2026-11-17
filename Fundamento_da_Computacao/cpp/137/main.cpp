#include "../bib1.hpp"
#include <vector>

int main() {
    while (true) {
        try {
            int nval = std::stoi(entrada());
            int n, mm;
            std::vector<std::vector<int>> m;
            if (nval > -1) {
                n = nval;
                mm = std::stoi(entrada());
                m.assign(n, std::vector<int>(mm));
                for (int i = 0; i < n; i++)
                    for (int j = 0; j < mm; j++)
                        m[i][j] = std::stoi(entrada());
            } else {
                n = 1000; mm = 1000;
                m.assign(n, std::vector<int>(mm));
                for (int i = 0; i < n; i++)
                    for (int j = 0; j < mm; j++)
                        m[i][j] = i*n+j;
            }
            std::cout << "[";
            for (int j = 0; j < mm; j++) {
                if (j > 0) std::cout << ", ";
                std::cout << "[";
                for (int i = 0; i < n; i++) {
                    if (i > 0) std::cout << ", ";
                    std::cout << m[i][j];
                }
                std::cout << "]";
            }
            std::cout << "]\n";
        } catch (...) { break; }
    }
}
