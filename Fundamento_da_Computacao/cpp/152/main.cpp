#include "../bib1.hpp"
#include <vector>
#include <sstream>
#include <iomanip>

int main() {
    while (true) {
        try {
            int nval = std::stoi(entrada());
            int n;
            std::vector<std::vector<int>> a;
            std::vector<int> b;
            if (nval >= 1) {
                n = nval;
                a.resize(n); b.resize(n);
                for (int i = 0; i < n; i++) {
                    std::string line = entrada();
                    std::istringstream iss(line);
                    a[i].resize(n);
                    for (int j = 0; j < n; j++) iss >> a[i][j];
                    iss >> b[i];
                }
            } else {
                n = 1000;
                a.resize(n); b.resize(n, 1);
                for (int i = 0; i < n; i++) { a[i].assign(i+1, 1); }
            }
            std::vector<double> x(n);
            for (int i = 0; i < n; i++) {
                double s = b[i];
                for (int j = 0; j < i; j++) s -= a[i][j] * x[j];
                x[i] = s / a[i][i];
            }
            for (int i = 0; i < n; i++) { if (i > 0) std::cout << " "; std::cout << std::fixed << std::setprecision(1) << x[i]; }
            std::cout << "\n";
        } catch (...) { break; }
    }
}
