#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int E;
    struct No * Prox;
} No;

typedef struct Pilha {
    No * Topo;
    int N;
} Pilha;

void ConstroiP(Pilha *P) {
    P->Topo = NULL; P->N = 0;
}

void Empilha(Pilha *P, int x) {
    No * n = (No *) malloc(sizeof(No));
    n->E = x;
    n->Prox = P->Topo;
    P->Topo = n;
    P->N++;
}

int Desempilha(Pilha *P) {
    No * n = P->Topo; int x = n->E;
    P->Topo = P->Topo->Prox;
    free(n);
    P->N--;
    return x;
}

int TopoP(Pilha *P) {
    return P->Topo->E;
}

int TamanhoP(Pilha *P) {
    return P->N;
}

typedef struct Fila {
    No * Prox;
    No * Ult;
    int N;
} Fila;

void ConstroiF(Fila *F) {
    F->Prox = NULL; F->Ult = NULL; F->N = 0;
}

void Enfileira(Fila *F, int x) {
    No * n = (No *) malloc(sizeof(No));
    n->E = x;
    n->Prox = NULL;
    if (F->Ult == NULL) {
        F->Prox = n;
        F->Ult = n;
    } else {
        F->Ult->Prox = n;
        F->Ult = n;
    }
    F->N++;
}

int Desenfileira(Fila *F) {
    No * n = F->Prox; int x = n->E;
    F->Prox = F->Prox->Prox;
    if (F->Prox == NULL) F->Ult = NULL;
    free(n);
    F->N--;
    return x;
}

int ProximoF(Fila *F) {
    return F->Prox->E;
}

int TamanhoF(Fila *F) {
    return F->N;
}

void AlternarPosNeg(int num[], int n) {
    Pilha pos, neg;
    ConstroiP(&pos); ConstroiP(&neg);
    for (int i = 0; i < n; i++) {
        if (num[i] >= 0) {
            Empilha(&pos, num[i]);
        } else {
            Empilha(&neg, num[i]);
        }
    }
    int i = 0;
    while (TamanhoP(&pos) > 0 || TamanhoP(&neg) > 0) {
        if (TamanhoP(&pos) > 0) {
            num[i++] = Desempilha(&pos);
        }
        if (TamanhoP(&neg) > 0) {
            num[i++] = Desempilha(&neg);
        }
    }
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    int * num; int n;
    while (scanf("%d", &n) > 0) {
        if (n > -1) {
            num = (int *) malloc(sizeof(int) * n);
            for (int i = 0; i < n; i++) {
                scanf("%d", &num[i]);
            }
            AlternarPosNeg(num, n);
            free(num);
        } else {
            if (n == -1) {
                n = 100000;
                num = (int *) malloc(sizeof(int) * n);
                int s = 1;
                for (int i = 0; i < n; i++) { num[i] = s*(i+1); s = -s; }
                AlternarPosNeg(num, n);
                free(num);
            } else if (n == -2) {
                n = 100000;
                num = (int *) malloc(sizeof(int) * n);
                for (int i = 0; i < n/2; i++) num[i] = (i+1);
                for (int i = n/2; i < n; i++) num[i] = -(i+1);
                AlternarPosNeg(num, n);
                free(num);
            } else {
                n = 100000;
                num = (int *) malloc(sizeof(int) * n);
                for (int i = 0; i < n/2; i++) num[i] = -(i+1);
                for (int i = n/2; i < n; i++) num[i] = (i+1);
                AlternarPosNeg(num, n);
                free(num);
            }
        }
    }
    return 0;
}
