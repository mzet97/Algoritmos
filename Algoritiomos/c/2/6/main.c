#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void RemoveMaiores(int L[], int *n, int k) {
    int j = 0;
    for (int i = 0; i < *n; i++) {
        if (L[i] <= k) {
            L[j] = L[i];
            j++;
        }
    }
    *n = j;
}

int cmp_int(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

void Insere(int L[], int *n, int e) {
    L[*n] = e;
    *n = *n + 1;
}

void Escreve(int L[], int n) {
    qsort(L, n, sizeof(int), cmp_int);
    printf("[");
    for (int i = 0; i < n; i++) printf("%d", L[i]);
    printf("]\n");
}

void C_RemoveMaiores(int L[], int *n, int k, int q) {
    for (int t = 0; t < q; t++) {
        RemoveMaiores(L, n, k);
    }
}

int main() {
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
    int max_n;
    int *L;
    int n;
    while (scanf("%d", &max_n) > 0) {
        if (max_n != -1) {
            L = (int *)malloc(sizeof(int) * max_n);
            n = 0;
            for (int i = 0; i < max_n; i++) {
                int e;
                scanf("%d", &e);
                Insere(L, &n, e);
            }
            int k;
            scanf("%d", &k);
            C_RemoveMaiores(L, &n, k, 1);
            Escreve(L, n);
            free(L);
        } else {
            max_n = 1000000;
            L = (int *)malloc(sizeof(int) * max_n);
            n = 0;
            for (int i = 0; i < max_n; i++) {
                Insere(L, &n, (i % 2 == 0 ? 2 : 3));
            }
            C_RemoveMaiores(L, &n, 2, 1);
            Escreve(L, n);
            free(L);
        }
    }
    return 0;
}
