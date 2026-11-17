#include "../bib1.h"
#include <stdlib.h>
#include <math.h>

int eh_primo(long long x) {
    if (x < 2) return 0;
    if (x == 2) return 1;
    if (x % 2 == 0) return 0;
    long long lim = (long long)sqrt((double)x);
    for (long long i = 3; i <= lim; i += 2) {
        if (x % i == 0) return 0;
    }
    return 1;
}

int main(void) {
    char buf[256];
    while (entrada(buf, sizeof(buf))) {
        long long x = atoll(buf);
        printf("%s\n", eh_primo(x) ? "True" : "False");
    }
    return 0;
}
