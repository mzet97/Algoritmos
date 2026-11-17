#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int E;
    struct No * Prox;
} No;

/* Count how many elements of LE (linked list) are contained in LS (sequential array of size N) */
int QuantosContidos(int LS[], int N, No * LE) {
    /* Build a hash set of LS values using a simple approach.
       Since N could be up to 3000 and n*n up to 9M, we need O(N*M) or better.
       Use a simple frequency array approach or linear search. */
    int count = 0;
    No * p = LE;
    while (p != NULL) {
        /* Check if p->E is in LS */
        for (int i = 0; i < N; i++) {
            if (LS[i] == p->E) {
                count++;
                break;
            }
        }
        p = p->Prox;
    }
    return count;
}

void InsereSequencialNaoOrdenada(int L[], int *n, int x) {
    L[*n] = x; *n = *n + 1;
}

void InsereEncadeadaNaoOrdenada(No ** L, int x) {
    No * novo = (No *) malloc(sizeof(No));
    novo->E = x; novo->Prox = *L; *L = novo;
}

void DesalocarEncadeada(No * L) {
    No * p;
    while (L != NULL) {
        p = L->Prox; free(L); L = p;
    }
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    int n; int * LS; No * LE; int x; int N;

    while (scanf("%d", &n)>0) {
        if (n>-1) {
            LS = (int *) malloc(sizeof(int)*n); N = 0;
            for (int i=0; i<n; i++) {
                scanf("%d", &x);
                InsereSequencialNaoOrdenada(LS, &N, x);
            }

            LE = NULL;
            for (int i=0; i<n*n; i++) {
                scanf("%d", &x);
                InsereEncadeadaNaoOrdenada(&LE, x);
            }
            printf("%d\n", QuantosContidos(LS, N, LE));
            free(LS); DesalocarEncadeada(LE);
        } else {
            /* automatic test */
            n = 3000;
            LS = (int *) malloc(sizeof(int)*n); N = 0;
            for (int i=0; i<n; i++) {
                InsereSequencialNaoOrdenada(LS, &N, 1);
            }
            LE = NULL;
            for (int i=0; i<n*n; i++) {
                InsereEncadeadaNaoOrdenada(&LE, 2);
            }
            printf("%d\n", QuantosContidos(LS, N, LE));
            free(LS); DesalocarEncadeada(LE);
        }
    }
    return 0;
}
