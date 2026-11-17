#include "../bib1.h"
#include <stdlib.h>

double troco(const int *m, const int *q, int n) {
    long long total = 0;
    for (int i = 0; i < n; i++) total += (long long)m[i] * q[i];
    return (double)total / 100.0;
}

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        int n = atoi(buf);
        int *m = malloc(n * sizeof(int)), *q = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) {
            if (!entrada(buf, sizeof(buf))) { free(m); free(q); return 0; }
            m[i] = atoi(buf);
        }
        for (int i = 0; i < n; i++) {
            if (!entrada(buf, sizeof(buf))) { free(m); free(q); return 0; }
            q[i] = atoi(buf);
        }
        print_python_float(troco(m, q, n)); printf("\n");
        free(m); free(q);
    }
    return 0;
}
