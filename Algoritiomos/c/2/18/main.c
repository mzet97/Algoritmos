#include <stdio.h>
#include <stdlib.h>

/* Concatenate two arrays: returns new array with L1 followed by L2 */
int * Concatenar(int L1[], int n1, int L2[], int n2) {
    int * L = (int *) malloc(sizeof(int) * (n1 + n2));
    for (int i = 0; i < n1; i++) {
        L[i] = L1[i];
    }
    for (int i = 0; i < n2; i++) {
        L[n1 + i] = L2[i];
    }
    return L;
}

void Insere(int L[], int *n, int e) {
    L[*n] = e; *n = *n + 1;
}

void Escreve(int L[], int n) {
    printf("["); for (int i=0; i<n; i++) printf("%d ", L[i]); printf("]\n");
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    int max_n1, max_n2; int * L1; int n1; int * L2; int n2; int * L;
    while (scanf("%d %d", &max_n1, &max_n2)>0) {
        if (max_n1!=-1) {
            L1 = (int *) malloc(sizeof(int)*max_n1); n1 = 0;
            for (int i=0; i<max_n1; i++) {
                int e; scanf("%d", &e); Insere(L1,&n1,e);
            }
            L2 = (int *) malloc(sizeof(int)*max_n2); n2 = 0;
            for (int i=0; i<max_n2; i++) {
                int e; scanf("%d", &e); Insere(L2,&n2,e);
            }
            L = Concatenar(L1,n1,L2,n2); Escreve(L,n1+n2);
            free(L);
        } else {
            max_n1 = 500000; max_n2 = max_n1;
            L1 = (int *) malloc(sizeof(int)*max_n1); n1 = 0;
            L2 = (int *) malloc(sizeof(int)*max_n2); n2 = 0;
            for (int i=0; i<max_n1; i++) {
                Insere(L1,&n1,1); Insere(L2,&n2,2);
            }
            L = Concatenar(L1,n1,L2,n2); Escreve(L,n1+n2);
            free(L);
        }
    }
    return 0;
}
