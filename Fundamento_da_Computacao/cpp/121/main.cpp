#include "../bib1.hpp"

long long mdc(long long a, long long b) {
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    while (b != 0) {
        long long temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

void reduz(long long p, long long q, long long &rp, long long &rq) {
    long long g = mdc(p, q);
    if (g == 0) {
        rp = p;
        rq = q;
    } else {
        rp = p / g;
        rq = q / g;
    }
}

int main() {
    while (true) {
        try {
            long long p = std::stoll(entrada());
            long long q = std::stoll(entrada());
            long long rp, rq;
            reduz(p, q, rp, rq);
            std::cout << "(" << rp << ", " << rq << ")\n";
        } catch (...) {
            break;
        }
    }
    return 0;
}
