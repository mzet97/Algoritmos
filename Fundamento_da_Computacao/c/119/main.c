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

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        long long x = atoll(buf);
        if (!entrada(buf, sizeof(buf))) break;
        long long y = atoll(buf);
        printf("%lld\n", mmc(x, y));
    }
    return 0;
}
