#include "../bib1.hpp"

int main() {
    while (true) {
        try {
            int a = std::stoi(entrada());
            int b = std::stoi(entrada());
            if (a == 0) {
                std::cout << "None" << "\n";
            } else {
                double result = static_cast<double>(-b) / a;
                std::cout << python_float(result) << "\n";
            }
        } catch (...) {
            break;
        }
    }
    return 0;
}
