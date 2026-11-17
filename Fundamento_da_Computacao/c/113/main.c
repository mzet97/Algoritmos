#include "../bib1.h"
#include <stdlib.h>

void conta_num(const int *lista, int n, int *qp, int *qn) {
    *qp = 0; *qn = 0;
    for (int i = 0; i < n; i++) {
        if (lista[i] > 0) (*qp)++;
        if (lista[i] < 0) (*qn)++;
    }
}

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        int n = atoi(buf);
        int *lista = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) {
            if (!entrada(buf, sizeof(buf))) { free(lista); return 0; }
            lista[i] = atoi(buf);
        }
        int qp, qn;
        conta_num(lista, n, &qp, &qn);
        printf("(%d, %d)\n", qp, qn);
        free(lista);
    }
    return 0;
}
