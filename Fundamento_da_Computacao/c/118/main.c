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

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        long long x = atoll(buf);
        if (!entrada(buf, sizeof(buf))) break;
        long long y = atoll(buf);
        printf("%lld\n", mdc(x, y));
    }
    return 0;
}
