#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int E;
    struct No * Prox;
} No;

/* Multiply polynomial P by scalar a */
No * ProdutoPolinomioEscalar(No * P, int a) {
    No * head = NULL;
    No * tail = NULL;
    No * p = P;
    while (p != NULL) {
        No * novo = (No *) malloc(sizeof(No));
        novo->E = p->E * a;
        novo->Prox = NULL;
        if (head == NULL) {
            head = novo;
        } else {
            tail->Prox = novo;
        }
        tail = novo;
        p = p->Prox;
    }
    return head;
}

void InsereEnc(No ** L, int e, No ** u) {
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
    printf("["); while (L != NULL) { printf("%d ", L->E); L = L->Prox; }; printf("]\n");
}

void Destroi(No * L) {
    No * p, * pr; p = L;
    while (p != NULL) {
        pr = p; p = p->Prox;
        free(pr);
    }
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    int n; No * P; No * u; int e; No * R; int a;
    while (scanf("%d", &n)>0) {
        P = NULL;
        if (n!=-1) {
            u = NULL;
            for (int i=0; i<=n; i++) {
                scanf("%d", &e);
                InsereEnc(&P,e,&u);
            }
            scanf("%d", &a);
        } else {
            n = 500000;
            u = NULL;
            for (int i=0; i<=n; i++) {
                InsereEnc(&P,1,&u);
            }
            a = 2;
        }
        R = ProdutoPolinomioEscalar(P, a);
        Escreve(R); Destroi(R); Destroi(P);
    }
    return 0;
}
