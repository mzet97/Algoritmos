#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int E;
    struct No * Prox;
} No;

/* Concatenate two linked lists: L1 followed by L2.
   Modifies L1 by appending L2 to its end. Returns new list. */
No * Concatenar(No * L1, No * L2) {
    if (L1 == NULL) return L2;
    if (L2 == NULL) return L1;

    /* Find the last node of L1 */
    No * p = L1;
    while (p->Prox != NULL) {
        p = p->Prox;
    }
    p->Prox = L2;
    return L1;
}

void Escreve(No * L) {
    printf("[");
    while (L != NULL) { printf("%d ", L->E); L = L->Prox; };
    printf("]\n");
}

void Destroi(No * L) {
    No * p, * pr; p = L;
    while (p != NULL) {
        pr = p; p = p->Prox;
        free(pr);
    }
}

void Insere(No ** L, int e, No ** u) {
    No * novo = (No *) malloc(sizeof(No));
    novo->E = e; novo->Prox = NULL;
    if (*u == NULL) { *L = novo; } else { (*u)->Prox = novo; };
    *u = novo;
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    int max_n1, max_n2; No * L1; No * L2; No * u; No * L;
    while (scanf("%d %d", &max_n1, &max_n2)>0) {
        if (max_n1!=-1) {
            L1 = NULL; u = NULL;
            for (int i=0; i<max_n1; i++) {
                int e; scanf("%d", &e); Insere(&L1,e,&u);
            }
            L2 = NULL; u = NULL;
            for (int i=0; i<max_n2; i++) {
                int e; scanf("%d", &e); Insere(&L2,e,&u);
            }
            L = Concatenar(L1,L2); Escreve(L);
            free(L);
        } else {
            max_n1 = max_n2 = 500000;
            L1 = NULL; u = NULL;
            for (int i=0; i<max_n1; i++) {
                int e; scanf("%d", &e); Insere(&L1,e,&u);
            }
            L2 = NULL; u = NULL;
            for (int i=0; i<max_n2; i++) {
                int e; scanf("%d", &e); Insere(&L2,e,&u);
            }
            L = Concatenar(L1,L2); Escreve(L);
            free(L);
        }
    }
    return 0;
}
