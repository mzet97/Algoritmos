#include "../bib1.hpp"

int main() {
    while (true) {
        try {
            int x = std::stoi(entrada());
            int y = std::stoi(entrada());
            std::string op = entrada();
            if (op == "+") {
                std::cout << x + y << "\n";
            } else if (op == "-") {
                std::cout << x - y << "\n";
            } else if (op == "x") {
                std::cout << x * y << "\n";
            } else if (op == "/") {
                std::cout << python_float(static_cast<double>(x) / y) << "\n";
            }
        } catch (...) {
            break;
        }
    }
    return 0;
}
