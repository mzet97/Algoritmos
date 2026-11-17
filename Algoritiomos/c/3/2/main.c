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
    if (P->Topo == NULL) return -1;
    No * n = P->Topo; int x = n->E;
    P->Topo = P->Topo->Prox;
    free(n);
    P->N--;
    return x;
}

int TopoP(Pilha *P) {
    if (P->Topo == NULL) return -1;
    return P->Topo->E;
}

int TamanhoP(Pilha *P) {
    return P->N;
}

typedef struct Fila {
    Pilha P1, P2;
} Fila;

void Constroi(Fila *F) {
    ConstroiP(&F->P1);
    ConstroiP(&F->P2);
}

void Enfileira(Fila *F, int x) {
    Empilha(&F->P1, x);
}

int Desenfileira(Fila *F) {
    if (TamanhoP(&F->P2) == 0) {
        while (TamanhoP(&F->P1) > 0) {
            Empilha(&F->P2, Desempilha(&F->P1));
        }
    }
    if (TamanhoP(&F->P2) == 0) return -1;
    return Desempilha(&F->P2);
}

int Proximo(Fila *F) {
    if (TamanhoP(&F->P2) == 0) {
        while (TamanhoP(&F->P1) > 0) {
            Empilha(&F->P2, Desempilha(&F->P1));
        }
    }
    if (TamanhoP(&F->P2) == 0) return -1;
    return TopoP(&F->P2);
}

int Tamanho(Fila *F) {
    return TamanhoP(&F->P1) + TamanhoP(&F->P2);
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    Fila F; Constroi(&F);
    char op[2]; int e;
    while (scanf("%1s", op) > 0) {
        if (op[0] == 'i') {
            scanf("%d", &e); Enfileira(&F, e);
        } else if (op[0] == 'r') {
            printf("%d\n", Desenfileira(&F));
        } else if (op[0] == 'p') {
            printf("%d\n", Proximo(&F));
        } else if (op[0] == 'n') {
            printf("%d\n", Tamanho(&F));
        } else {
            for (int i = 1; i <= 1000; i++) {
                Enfileira(&F, i);
            }
            while (Tamanho(&F) > 0) {
                printf("%d ", Tamanho(&F));
                printf("%d ", Proximo(&F));
                printf("%d ", Desenfileira(&F));
            }
            printf("\n");
        }
    }
    return 0;
}

