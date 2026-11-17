#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int E;
    struct No * Prox;
} No;

/* Remove node p from a singly linked list.
   Since we can't go backwards, we copy the next node's data to p and remove next.
   Special case: p is the last node - must search from head. */
void Remove(No ** L, No * p) {
    if (p->Prox != NULL) {
        /* Copy next node into p, then free next */
        No * temp = p->Prox;
        p->E = temp->E;
        p->Prox = temp->Prox;
        free(temp);
    } else {
        /* p is the last node - must search from head */
        if (*L == p) {
            /* p is the only node */
            free(p);
            *L = NULL;
        } else {
            No * prev = *L;
            while (prev->Prox != p) {
                prev = prev->Prox;
            }
            prev->Prox = NULL;
            free(p);
        }
    }
}

void C_Remove(No ** L, No * p, int q) {
#ifndef CODEV
    printf("CODEV_BEGIN_EXEC\n");
#endif
    for (int t=0; t<q; t++) {
        Remove(L, p);
    }
#ifndef CODEV
    printf("CODEV_END_EXEC\n");
#endif
}

void Insere(No ** L, int e) {
    No * novo = (No *) malloc(sizeof(No));
    novo->E = e; novo->Prox = *L; *L = novo;
}

void Escreve(No * L) {
    No * p = L;
    printf("[ "); for (p=L; p!=NULL; p=p->Prox) printf("%d ", p->E); printf("]\n");
}

void Destroi(No ** L) {
    No * p, * pr; p = *L;
    while (p != NULL) {
        pr = p; p = p->Prox; free(pr);
    }
    *L = NULL;
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    int n; No * L = NULL; No * Lr;
    while (scanf("%d", &n)>0) {
        if (n!=-1) {
            No ** V = (No **) malloc(sizeof(No *) * n);
            for (int i=0; i<n; i++) {
                int e; scanf("%d", &e); Insere(&L,e); V[i] = L;
            }
            int i; scanf("%d", &i);
            C_Remove(&L,V[i-1],1); Escreve(L); Destroi(&L);
            free(V);
        } else {
            n = 100000;
            No ** V = (No **) malloc(sizeof(No *) * n);
            for (int i=0; i<n; i++) {
                Insere(&L,i%100); V[i] = L;
            }
            for (int i=0; i<10; i++) {
                C_Remove(&L,V[i*9000],1);
            }
            Escreve(L); Destroi(&L);
            free(V);
        }
    }
    return 0;
}
