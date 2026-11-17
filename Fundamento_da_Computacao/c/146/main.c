#include "../bib1.h"

long long fibonacci(int n) {
    if (n <= 0) return 0;
    if (n <= 2) return 1;
    long long a = 1, b = 1;
    for (int i = 2; i < n; i++) { long long t = a + b; a = b; b = t; }
    return b;
}

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        int n = atoi(buf);
        printf("%lld\n", fibonacci(n));
    }
    return 0;
}
