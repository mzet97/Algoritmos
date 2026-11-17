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
            long long p1 = std::stoll(entrada());
            long long q1 = std::stoll(entrada());
            long long p2 = std::stoll(entrada());
            long long q2 = std::stoll(entrada());
            long long num = p1 * p2;
            long long den = q1 * q2;
            long long rp, rq;
            reduz(num, den, rp, rq);
            std::cout << "(" << rp << ", " << rq << ")\n";
        } catch (...) {
            break;
        }
    }
    return 0;
}
