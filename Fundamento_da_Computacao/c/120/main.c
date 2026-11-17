#include "../bib1.h"
#include <stdlib.h>

long long mdc(long long x, long long y) {
    while (y != 0) {
        long long t = y;
        y = x % y;
        x = t;
    }
    return x;
}

long long mmc(long long x, long long y) {
    return (x / mdc(x, y)) * y;
}

long long multiplos(long long a, long long b, long long n) {
    long long lcm = mmc(a, b);
    if (lcm >= n) return 0;
    return (n - 1) / lcm;
}

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        long long a = atoll(buf);
        if (!entrada(buf, sizeof(buf))) break;
        long long b = atoll(buf);
        if (!entrada(buf, sizeof(buf))) break;
        long long n = atoll(buf);
        printf("%lld\n", multiplos(a, b, n));
    }
    return 0;
}
