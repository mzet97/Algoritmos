#include "../bib1.h"

static long long gcd(long long a, long long b) {
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    while (b) {
        long long t = b;
        b = a % b;
        a = t;
    }
    return a;
}

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        long long p1 = atoll(buf);
        if (!entrada(buf, sizeof(buf))) break;
        long long q1 = atoll(buf);
        if (!entrada(buf, sizeof(buf))) break;
        long long p2 = atoll(buf);
        if (!entrada(buf, sizeof(buf))) break;
        long long q2 = atoll(buf);

        long long p = p1 * q2;
        long long q = q1 * p2;

        long long g = gcd(p, q);
        p /= g;
        q /= g;

        printf("(%lld, %lld)\n", p, q);
    }
    return 0;
}
