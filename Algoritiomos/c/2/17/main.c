#include <stdio.h>
#include <stdlib.h>

/* Reverse array in place */
void Inverter(int L[], int n) {
    for (int i = 0; i < n / 2; i++) {
        int temp = L[i];
        L[i] = L[n - 1 - i];
        L[n - 1 - i] = temp;
    }
}

void Insere(int L[], int *n, int e) {
    L[*n] = e; *n = *n + 1;
}

void Escreve(int L[], int *n) {
    printf("["); for (int i=0; i<*n; i++) printf("%d ", L[i]); printf("]\n");
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    int max_n; int * L; int n;
    while (scanf("%d", &max_n)>0) {
        if (max_n!=-1) {
            L = (int *) malloc(sizeof(int)*max_n); n = 0;
            for (int i=0; i<max_n; i++) {
                int e; scanf("%d", &e); Insere(L,&n,e);
            }
            Inverter(L,n); Escreve(L,&n);
            free(L);
        } else {
            max_n = 1000000;
            L = (int *) malloc(sizeof(int)*max_n); n = 0;
            for (int i=0; i<max_n; i++) {
                Insere(L,&n,i%10);
            }
            Inverter(L,n); Escreve(L,&n);
            free(L);
        }
    }
    return 0;
}
