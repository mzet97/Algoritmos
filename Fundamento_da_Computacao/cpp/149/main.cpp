#include "../bib1.hpp"
#include <vector>

long long polinomio(int N, const std::vector<int>& a, int k) {
    // Horner's method: f(k) = (...((a[N]*k + a[N-1])*k + a[N-2])*k + ...)*k + a[0]
    long long result = a[N];
    for (int i = N - 1; i >= 0; i--) {
        result = result * k + a[i];
    }
    return result;
}

int main() {
    while (true) {
        try {
            int N = std::stoi(entrada());
            std::vector<int> a;
            int k;
            if (N >= 0) {
                for (int i = 0; i <= N; i++) {
                    a.push_back(std::stoi(entrada()));
                }
                k = std::stoi(entrada());
            } else {
                N = 100000;
                a.assign(N + 1, 1);
                k = 1;
            }
            std::cout << polinomio(N, a, k) << "\n";
        } catch (...) {
            break;
        }
    }
    return 0;
}
