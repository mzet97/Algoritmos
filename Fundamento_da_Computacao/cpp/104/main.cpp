#include "../bib1.hpp"

int contagem(int x, int y) {
    std::string s = std::to_string(std::abs(x));
    char target = '0' + (std::abs(y) % 10);
    int count = 0;
    for (char c : s) if (c == target) count++;
    return count;
}

int main() {
    while (true) {
        try {
            int x = std::stoi(entrada());
            int y = std::stoi(entrada());
            std::cout << contagem(x, y) << "\n";
        } catch (...) { break; }
    }
}
