#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int E;
    struct No * Prox;
} No;

int Ultimo(No * L) {
    if (L == NULL) return -1;
    No * p = L->Prox;
    while (p->Prox != L) {
        p = p->Prox;
    }
    return p->E;
}

void Insere(No ** L, int e, No ** u) {
    No * novo = (No *) malloc(sizeof(No));
    novo->E = e;
    if (*L == NULL) {
        novo->Prox = novo;
        *L = novo;
    } else {
        novo->Prox = *L;
        (*u)->Prox = novo;
    }
    *u = novo;
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    No * L = NULL; No * u;
    char op[2]; int e, k;
    while (scanf("%1s", op)>0) {
        if (op[0] != 'a') {
            if (op[0] == 'i') {
                scanf("%d", &e); Insere(&L,e,&u);
            } else {
                printf("%d\n", Ultimo(L));
            }
        } else {
            for (int i=1; i<=2000000; i++) {
                Insere(&L,i,&u);
            }
            printf("%d\n", Ultimo(L));
        }
    }
    return 0;
}

