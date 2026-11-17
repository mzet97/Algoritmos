#include "../bib1.hpp"
#include <vector>

bool maiorigual_lista(const std::vector<int>& x, const std::vector<int>& y) {
    // x and y are digit lists with least significant digit first
    // Return true if number(x) >= number(y)
    int nx = static_cast<int>(x.size());
    int ny = static_cast<int>(y.size());
    if (nx != ny) return nx > ny;
    // Same length: compare from most significant digit (end of list)
    for (int i = nx - 1; i >= 0; i--) {
        if (x[i] != y[i]) return x[i] > y[i];
    }
    return true; // numbers are equal
}

int main() {
    while (true) {
        try {
            std::string xs = entrada();
            std::string ys = entrada();
            std::vector<int> x_lista, y_lista;
            // reversed(x) means iterate from end to start
            for (int i = static_cast<int>(xs.size()) - 1; i >= 0; i--) {
                x_lista.push_back(xs[i] - '0');
            }
            for (int i = static_cast<int>(ys.size()) - 1; i >= 0; i--) {
                y_lista.push_back(ys[i] - '0');
            }
            if (maiorigual_lista(x_lista, y_lista)) {
                std::cout << "1\n";
            } else {
                std::cout << "0\n";
            }
        } catch (...) {
            break;
        }
    }
    return 0;
}
