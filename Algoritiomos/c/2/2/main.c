#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int E;
    struct No * Prox;
} No;

int Maior(No * L) {
    if (L == NULL) return -1;
    int maior = L->E;
    No * p = L->Prox;
    while (p != NULL) {
        if (p->E > maior) {
            maior = p->E;
        }
        p = p->Prox;
    }
    return maior;
}

void Insere(No ** L, int e) {
    No * novo = (No *) malloc(sizeof(No));
    novo->E = e;
    novo->Prox = *L;
    *L = novo;
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    No * L = NULL;
    char op[2]; int e, k;
    while (scanf("%1s", op)>0) {
        if (op[0] != 'a') {
            if (op[0] == 'i') {
                scanf("%d", &e); Insere(&L,e);
            } else {
                printf("%d\n", Maior(L));
            }
        } else {
            int n = 1000000;
            for (int i=n/2; i<=n; i++) {
                Insere(&L,i);
            }
            for (int i=1; i < n/2; i++) {
                Insere(&L,i);
            }
            printf("%d\n", Maior(L));
        }
    }
    return 0;
}

