#include "../bib1.h"
#include <stdlib.h>
#include <string.h>

int main(void) {
    char buf[4096];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        int nval = atoi(buf);
        int **a; int *b; int n;
        if (nval >= 1) {
            n = nval;
            a = malloc(n * sizeof(int*));
            b = malloc(n * sizeof(int));
            for (int i = 0; i < n; i++) {
                if (!entrada(buf, sizeof(buf))) return 0;
                a[i] = malloc(n * sizeof(int));
                char *p = strtok(buf, " ");
                for (int j = 0; j < n; j++) { a[i][j] = atoi(p); p = strtok(NULL, " "); }
                b[i] = atoi(p);
            }
        } else {
            n = 1000;
            a = malloc(n * sizeof(int*));
            b = malloc(n * sizeof(int));
            for (int i = 0; i < n; i++) {
                a[i] = malloc((i+1) * sizeof(int));
                for (int j = 0; j <= i; j++) a[i][j] = 1;
                b[i] = 1;
            }
        }
        double *x = malloc(n * sizeof(double));
        for (int i = 0; i < n; i++) {
            double s = b[i];
            for (int j = 0; j < i; j++) s -= a[i][j] * x[j];
            x[i] = s / a[i][i];
        }
        for (int i = 0; i < n; i++) { if (i > 0) printf(" "); printf("%.1f", x[i]); }
        printf("\n");
        for (int i = 0; i < n; i++) free(a[i]); free(a); free(b); free(x);
    }
    return 0;
}
