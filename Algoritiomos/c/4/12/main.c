#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int E;
    struct No * Prox;
} No;

int NumeroElementos(No * L) {
    if (L == NULL) return 0;
    return 1 + NumeroElementos(L->Prox);
}

int KUltimoAux(No * L, int pos, int alvo) {
    if (L == NULL) return -1;
    if (pos == alvo) return L->E;
    return KUltimoAux(L->Prox, pos + 1, alvo);
}

int KUltimo(No * L, int k) {
    int n = NumeroElementos(L);
    if (k <= 0 || k > n) return -1;
    int alvo = n - k;
    return KUltimoAux(L, 0, alvo);
}

void Constroi(No ** L, No ** u) {
    *L = NULL; *u = NULL;
}

void Insere(No ** L, int e, No ** u) {
    No * novo = (No *) malloc(sizeof(No));
    novo->E = e;
    novo->Prox = NULL;
    if (*L == NULL) {
        *L = novo;
    } else {
        (*u)->Prox = novo;
    }
    *u = novo;
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    No * L = NULL; No * u; Constroi(&L, &u);
    char op[2]; int e, k;
    while (scanf("%1s", op) > 0) {
        if (op[0] != 'a') {
            if (op[0] == 'i') {
                scanf("%d", &e); Insere(&L, e, &u);
            } else {
                scanf("%d", &k); printf("%d\n", KUltimo(L, k));
            }
        } else {
            int n = 50000;
            for (int i = 1; i <= n; i++) {
                Insere(&L, i, &u);
            }
            for (int t = 1; t <= 100; t++) KUltimo(L, n/2);
            printf("%d\n", KUltimo(L, n/2));
        }
    }
    return 0;
}

