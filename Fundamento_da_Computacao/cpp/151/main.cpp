#include "../bib1.hpp"
#include <vector>
#include <sstream>

int main() {
    while (true) {
        try {
            int r = std::stoi(entrada()), s = std::stoi(entrada()), t = std::stoi(entrada());
            std::vector<std::vector<int>> m1(r, std::vector<int>(s));
            std::vector<std::vector<int>> m2(s, std::vector<int>(t));
            for (int i = 0; i < r; i++) {
                std::string line = entrada();
                std::istringstream iss(line);
                for (int j = 0; j < s; j++) iss >> m1[i][j];
            }
            for (int i = 0; i < s; i++) {
                std::string line = entrada();
                std::istringstream iss(line);
                for (int j = 0; j < t; j++) iss >> m2[i][j];
            }
            for (int i = 0; i < r; i++) {
                if (i > 0) std::cout << " | ";
                for (int j = 0; j < t; j++) {
                    int val = 0;
                    for (int k = 0; k < s; k++) val += m1[i][k] * m2[k][j];
                    if (j > 0) std::cout << " ";
                    std::cout << val;
                }
            }
            std::cout << "\n";
        } catch (...) { break; }
    }
}
