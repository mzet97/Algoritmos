#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int E;
    struct No * Prox;
} No;

/* Reverse a singly linked list iteratively */
No * Inverte(No * L) {
    No * prev = NULL;
    No * curr = L;
    while (curr != NULL) {
        No * next = curr->Prox;
        curr->Prox = prev;
        prev = curr;
        curr = next;
    }
    return prev;
}

No * C_Inverte(No * L, int q) {
#ifndef CODEV
    printf("CODEV_BEGIN_EXEC\n");
#endif
    No * r = NULL;
    for (int t=0; t<q; t++) {
        r = Inverte(L);
    }
#ifndef CODEV
    printf("CODEV_END_EXEC\n");
#endif
    return r;
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
            Lr = C_Inverte(L, 1); Escreve(Lr); Destroi(&Lr); Destroi(&L);
        } else {
            n = 500000;
            for (int i=0; i<n; i++) {
                Insere(&L,i%100);
            }
            Lr = C_Inverte(L, 1); Escreve(Lr); Destroi(&Lr); Destroi(&L);
        }
    }
    return 0;
}
