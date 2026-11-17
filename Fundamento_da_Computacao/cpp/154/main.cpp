#include "../bib1.hpp"

double calc_dist(double v, double a, double t) {
    // General acceleration: velocity may become zero and object reverses
    // Total distance traveled (path length)
    if (a < 0) {
        double t_stop = -v / a; // time when velocity reaches 0
        if (t <= t_stop) {
            return v * t + 0.5 * a * t * t;
        } else {
            // Distance in forward phase
            double d_forward = v * t_stop + 0.5 * a * t_stop * t_stop;
            // Distance in backward phase (absolute)
            double dt = t - t_stop;
            double d_backward = 0.5 * std::abs(a) * dt * dt;
            return d_forward + d_backward;
        }
    }
    return v * t + 0.5 * a * t * t;
}

int main() {
    while (true) {
        try {
            double v = std::stod(entrada());
            double a = std::stod(entrada());
            double t = std::stod(entrada());
            std::cout << python_float(calc_dist(v, a, t)) << "\n";
        } catch (...) {
            break;
        }
    }
    return 0;
}
