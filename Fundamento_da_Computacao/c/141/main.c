#include "../bib1.h"
#include <stdlib.h>
#include <float.h>

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        int nval = atoi(buf);
        int *m; double *notas; int n;
        if (nval >= 0) {
            n = nval;
            m = malloc(n * sizeof(int));
            notas = malloc(n * sizeof(double));
            for (int i = 0; i < n; i++) {
                if (!entrada(buf, sizeof(buf))) return 0;
                m[i] = atoi(buf);
            }
            for (int i = 0; i < n; i++) {
                if (!entrada(buf, sizeof(buf))) return 0;
                notas[i] = atof(buf);
            }
        } else {
            n = 1000000;
            m = malloc(n * sizeof(int));
            notas = malloc(n * sizeof(double));
            for (int i = 0; i < n; i++) m[i] = i;
            for (int i = 0; i < 999998; i++) notas[i] = 2.0;
            notas[999998] = 1.0; notas[999999] = 3.0;
        }
        double maxN = -DBL_MAX, minN = DBL_MAX;
        for (int i = 0; i < n; i++) {
            if (notas[i] > maxN) maxN = notas[i];
            if (notas[i] < minN) minN = notas[i];
        }
        printf("([");
        int first = 1;
        for (int i = 0; i < n; i++) if (notas[i] == maxN) { if (!first) printf(", "); printf("%d", m[i]); first = 0; }
        printf("], [");
        first = 1;
        for (int i = 0; i < n; i++) if (notas[i] == minN) { if (!first) printf(", "); printf("%d", m[i]); first = 0; }
        printf("])\n");
        free(m); free(notas);
    }
    return 0;
}
