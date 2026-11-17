#include "../bib1.h"
#include <stdlib.h>

long long polinomio(int N, int *a, int k) {
    /* f(x) = a[N]*x^N + a[N-1]*x^(N-1) + ... + a[1]*x + a[0] */
    /* Using Horner's method: */
    long long result = 0;
    for (int i = N; i >= 0; i--) {
        result = result * k + a[i];
    }
    return result;
}

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        int N = atoi(buf);
        int *a;
        int k;
        if (N >= 0) {
            a = (int *)malloc((N + 1) * sizeof(int));
            for (int i = 0; i <= N; i++) {
                if (!entrada(buf, sizeof(buf))) { free(a); return 0; }
                a[i] = atoi(buf);
            }
            if (!entrada(buf, sizeof(buf))) { free(a); return 0; }
            k = atoi(buf);
        } else {
            N = 100000;
            a = (int *)malloc((N + 1) * sizeof(int));
            for (int i = 0; i <= N; i++) a[i] = 1;
            k = 1;
        }
        printf("%lld\n", polinomio(N, a, k));
        free(a);
    }
    return 0;
}
