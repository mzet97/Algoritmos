#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int E;
    struct No * Prox;
} No;

/* List with sentinel head and tail nodes.
   L is the head sentinel (L->Prox is first real element),
   last real element's Prox is the tail sentinel.
   If found: remove x and return 1. If not found: insert x and return 0. */
int InsereBuscaRemove(No * L, int x) {
    /* Search for x in the list (between head and tail sentinels) */
    No * prev = L;
    No * p = L->Prox;
    /* Tail sentinel has Prox == NULL; traverse while p is not tail */
    while (p->Prox != NULL) {
        if (p->E == x) {
            /* Found: remove it */
            prev->Prox = p->Prox;
            free(p);
            return 1;
        }
        prev = p;
        p = p->Prox;
    }
    /* Not found: insert before tail (p is the tail sentinel here) */
    No * novo = (No *) malloc(sizeof(No));
    novo->E = x;
    novo->Prox = p;
    prev->Prox = novo;
    return 0;
}

void Insere(No ** L, int e, No ** u) {
    No * novo = (No *) malloc(sizeof(No));
    novo->E = e; novo->Prox = NULL;
    if (*u == NULL) {
        *L = novo;
    } else {
        (*u)->Prox = novo;
    }
    *u = novo;
}

void Escreve(No * L) {
    No * p = L->Prox;
    printf("[");
    while (p->Prox != NULL) { printf("%d ", p->E); p = p->Prox; };
    printf("]\n");
}

void Destroi(No ** L) {
    No * p, * pr; p = *L;
    while (p != NULL) {
        pr = p; p = p->Prox;
        free(pr);
    }
    *L = NULL;
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    int n; No * L; No * u; int x; int r;
    while (scanf("%d", &n)>0) {
        L = NULL; u = NULL;
        if (n!=-1) {
            Insere(&L,3000,&u);
            for (int i=0; i<n; i++) {
                scanf("%d", &x); Insere(&L,x,&u);
            }
            Insere(&L,3000,&u);
            scanf("%d", &x);
            r = InsereBuscaRemove(L, x);
            printf("%d ", r); Escreve(L); Destroi(&L);
        } else {
            n = 500000;
            Insere(&L,3000,&u);
            for (int i=0; i<n; i++) {
                Insere(&L,i,&u);
            }
            Insere(&L,3000,&u);
            r = InsereBuscaRemove(L, 300000);
            printf("%d ", r); Escreve(L); Destroi(&L);
        }
    }
    return 0;
}
