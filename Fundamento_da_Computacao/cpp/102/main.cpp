#include "../bib1.hpp"
#include <iomanip>

std::string doishorarios(int h, int m) {
    int minutos = m * 5;
    if (m == 12) minutos = 0;
    int hora = h;
    if (h == 12) hora = 0;

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hora << ":"
        << std::setfill('0') << std::setw(2) << minutos
        << " / "
        << std::setfill('0') << std::setw(2) << (hora + 12) << ":"
        << std::setfill('0') << std::setw(2) << minutos;
    return oss.str();
}

int main() {
    while (true) {
        try {
            int h = std::stoi(entrada());
            int m = std::stoi(entrada());
            std::cout << doishorarios(h, m) << "\n";
        } catch (...) {
            break;
        }
    }
    return 0;
}
