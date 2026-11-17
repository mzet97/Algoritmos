#include "../bib1.h"
#include <stdlib.h>
#include <string.h>

int main(void) {
    char buf[4096];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        int r = atoi(buf);
        if (!entrada(buf, sizeof(buf))) break;
        int s = atoi(buf);
        if (!entrada(buf, sizeof(buf))) break;
        int t = atoi(buf);
        int **m1 = malloc(r * sizeof(int*));
        for (int i = 0; i < r; i++) {
            if (!entrada(buf, sizeof(buf))) return 0;
            m1[i] = malloc(s * sizeof(int));
            char *p = strtok(buf, " ");
            for (int j = 0; j < s && p; j++) { m1[i][j] = atoi(p); p = strtok(NULL, " "); }
        }
        int **m2 = malloc(s * sizeof(int*));
        for (int i = 0; i < s; i++) {
            if (!entrada(buf, sizeof(buf))) return 0;
            m2[i] = malloc(t * sizeof(int));
            char *p = strtok(buf, " ");
            for (int j = 0; j < t && p; j++) { m2[i][j] = atoi(p); p = strtok(NULL, " "); }
        }
        for (int i = 0; i < r; i++) {
            if (i > 0) printf(" | ");
            for (int j = 0; j < t; j++) {
                int val = 0;
                for (int k = 0; k < s; k++) val += m1[i][k] * m2[k][j];
                if (j > 0) printf(" ");
                printf("%d", val);
            }
        }
        printf("\n");
        for (int i = 0; i < r; i++) free(m1[i]); free(m1);
        for (int i = 0; i < s; i++) free(m2[i]); free(m2);
    }
    return 0;
}
