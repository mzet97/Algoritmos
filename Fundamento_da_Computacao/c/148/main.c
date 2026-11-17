#include "../bib1.h"

long long fatorial(int n) {
    long long r = 1;
    for (int i = 2; i <= n; i++) r *= i;
    return r;
}

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        int n = atoi(buf);
        printf("%lld\n", fatorial(n));
    }
    return 0;
}
