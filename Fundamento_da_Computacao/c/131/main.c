#include "../bib1.h"
#include <stdlib.h>

int acima_media(const int *l, int n) {
    if (n == 0) return 0;
    long long soma = 0;
    for (int i = 0; i < n; i++) soma += l[i];
    double media = (double)soma / n;
    int count = 0;
    for (int i = 0; i < n; i++)
        if (l[i] > media) count++;
    return count;
}

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        if (strcmp(buf, "@entradagrande") == 0) {
            int *l = malloc(1000000 * sizeof(int));
            for (int i = 0; i < 1000000; i++) l[i] = i;
            printf("%d\n", acima_media(l, 1000000));
            free(l);
        } else {
            int n = atoi(buf);
            int *l = malloc(n * sizeof(int));
            for (int i = 0; i < n; i++) {
                if (!entrada(buf, sizeof(buf))) { free(l); return 0; }
                l[i] = atoi(buf);
            }
            printf("%d\n", acima_media(l, n));
            free(l);
        }
    }
    return 0;
}
