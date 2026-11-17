#include "../bib1.h"
#include <stdlib.h>

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        int n = atoi(buf);
        long long *fibs = malloc(n * sizeof(long long));
        long long a = 1, b = 1;
        for (int i = 0; i < n; i++) { fibs[i] = a; long long t = a+b; a = b; b = t; }
        for (int k = 1; k < n; k++) {
            for (int i = 0; i < k; i++) { if (i > 0) printf(" "); printf("%lld", fibs[i]); }
            printf("\n");
        }
        for (int i = 0; i < n; i++) { if (i > 0) printf(" "); printf("%lld", fibs[i]); }
        printf("\n");
        for (int k = n-1; k >= 1; k--) {
            for (int i = 0; i < k; i++) { if (i > 0) printf(" "); printf("%lld", fibs[i]); }
            printf("\n");
        }
        free(fibs);
    }
    return 0;
}
