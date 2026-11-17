#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int E;
    struct No *Ant;
    struct No *Prox;
} No;

int KUltimo(No *L, int k) {
    if (L == NULL || k <= 0) return -1;
    No *p = L;
    for (int i = 1; i <= k; i++) {
        p = p->Ant;
        if (p == L) {
            if (i < k) {
                /* wrapped around back to start: k > n */
                return -1;
            }
        }
    }
    return p->E;
}

void Insere(No **L, int e) {
    No *novo = (No *)malloc(sizeof(No));
    novo->E = e;
    if (*L == NULL) {
        novo->Prox = novo;
        novo->Ant = novo;
        *L = novo;
    } else {
        novo->Ant = (*L)->Ant;
        (*L)->Ant->Prox = novo;
        novo->Prox = *L;
        (*L)->Ant = novo;
        *L = novo;
    }
}

int main() {
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
    No *L = NULL;
    char op[3];
    int e, k;
    while (scanf("%2s", op) > 0) {
        if (op[0] != 'a') {
            if (op[0] == 'i') {
                scanf("%d", &e);
                Insere(&L, e);
            } else {
                scanf("%d", &k);
                printf("%d\n", KUltimo(L, k));
            }
        } else {
            int n = 2000000;
            for (int i = 1; i <= n; i++) {
                Insere(&L, i);
            }
            if (op[1] == '1') {
                printf("%d\n", KUltimo(L, n / 2));
            } else {
                printf("%d\n", KUltimo(L, 3));
                for (int i = 1; i <= 1000; i++) {
                    KUltimo(L, 3);
                }
            }
        }
    }
    return 0;
}

