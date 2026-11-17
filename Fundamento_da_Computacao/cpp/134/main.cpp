#include "../bib1.hpp"

double menor_angulo(int H, int M) {
    double hour_pos = (H % 12) * 30.0 + M * 0.5;
    double min_pos = M * 6.0;
    double angle = std::abs(hour_pos - min_pos);
    if (angle > 180.0) angle = 360.0 - angle;
    return angle;
}

int main() {
    while (true) {
        try {
            int H = std::stoi(entrada());
            int M = std::stoi(entrada());
            std::cout << python_float(menor_angulo(H, M)) << "\n";
        } catch (...) {
            break;
        }
    }
    return 0;
}
