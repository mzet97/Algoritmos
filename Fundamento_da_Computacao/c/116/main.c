#include "../bib1.h"
#include <stdlib.h>

long long raiz(long long n) {
    long long i = 0;
    while ((i + 1) * (i + 1) <= n) {
        i++;
    }
    return i;
}

int main(void) {
    char buf[256];
    while (entrada(buf, sizeof(buf))) {
        long long n = atoll(buf);
        printf("%lld\n", raiz(n));
    }
    return 0;
}
