#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int E;
    struct No * Prox;
} No;

/* Soma2a2: for each consecutive pair (a_i, a_{i+1}), create a node with a_i + a_{i+1}.
   If odd number of elements, the last element is paired with itself. */
No * Soma2a2(No * L) {
    No * result = NULL;
    No * tail = NULL;
    No * p = L;

    while (p != NULL) {
        No * novo = (No *) malloc(sizeof(No));
        if (p->Prox != NULL) {
            novo->E = p->E + p->Prox->E;
            p = p->Prox->Prox;
        } else {
            novo->E = p->E + p->E;
            p = p->Prox;
        }
        novo->Prox = NULL;
        if (result == NULL) {
            result = novo;
        } else {
            tail->Prox = novo;
        }
        tail = novo;
    }
    return result;
}

No * C_Soma2a2(No * L) {
#ifndef CODEV
    printf("CODEV_BEGIN_EXEC\n");
#endif
    No * r;
    r = Soma2a2(L);
#ifndef CODEV
    printf("CODEV_END_EXEC\n");
#endif
    return r;
}

int cmp_int(const void *a, const void *b) {
    return (*(int*)a) - (*(int*)b);
}

void Escreve(No * L) {
    int n = 0;
    No * p = L;
    while (p != NULL) {
        n++; p = p->Prox;
    }
    int * Lv = (int *) malloc(sizeof(int)*n);
    p = L; int i = 0;
    while (p != NULL) {
        Lv[i] = p->E; i++; p = p->Prox;
    }
    qsort(Lv, n, sizeof(int), cmp_int);
    printf("[ "); for (int i=0; i<n; i++) printf("%d ", Lv[i]); printf("]\n");
    free(Lv);
}

void Destroi(No ** L) {
    No * p, * pr; p = *L;
    while (p != NULL) {
        pr = p; p = p->Prox;
        free(pr);
    }
    *L = NULL;
}

void Insere(No ** L, int e) {
    No * novo = (No *) malloc(sizeof(No));
    novo->E = e; novo->Prox = *L; *L = novo;
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    int n; No * L = NULL; No * Lr;
    while (scanf("%d", &n)>0) {
        if (n!=-1) {
            for (int i=0; i<n; i++) {
                int e; scanf("%d", &e); Insere(&L,e);
            }
            Lr = C_Soma2a2(L); Escreve(Lr); Destroi(&Lr); Destroi(&L);
        } else {
            n = 1000;
            for (int i=0; i<n; i++) {
                Insere(&L,i);
            }
            Lr = C_Soma2a2(L); Escreve(Lr); Destroi(&Lr); Destroi(&L);
        }
    }
    return 0;
}
