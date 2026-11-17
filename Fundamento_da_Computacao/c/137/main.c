#include "../bib1.h"
#include <stdlib.h>

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        int n = atoi(buf);
        int mm;
        int **m;
        if (n > -1) {
            if (!entrada(buf, sizeof(buf))) break;
            mm = atoi(buf);
            m = malloc(n * sizeof(int*));
            for (int i = 0; i < n; i++) {
                m[i] = malloc(mm * sizeof(int));
                for (int j = 0; j < mm; j++) {
                    if (!entrada(buf, sizeof(buf))) return 0;
                    m[i][j] = atoi(buf);
                }
            }
        } else {
            n = 1000; mm = 1000;
            m = malloc(n * sizeof(int*));
            for (int i = 0; i < n; i++) {
                m[i] = malloc(mm * sizeof(int));
                for (int j = 0; j < mm; j++) m[i][j] = i*n+j;
            }
        }
        /* transpose */
        printf("[");
        for (int j = 0; j < mm; j++) {
            if (j > 0) printf(", ");
            printf("[");
            for (int i = 0; i < n; i++) {
                if (i > 0) printf(", ");
                printf("%d", m[i][j]);
            }
            printf("]");
        }
        printf("]\n");
        for (int i = 0; i < n; i++) free(m[i]);
        free(m);
    }
    return 0;
}
