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

void ConstroiP(Pilha *P) { P->Topo = NULL; P->N = 0; }
void Empilha(Pilha *P, int x) {
    No * n = (No *) malloc(sizeof(No));
    n->E = x; n->Prox = P->Topo; P->Topo = n; P->N++;
}
int Desempilha(Pilha *P) {
    No * n = P->Topo; int x = n->E;
    P->Topo = P->Topo->Prox; free(n); P->N--;
    return x;
}
int TopoP(Pilha *P) { return P->Topo->E; }
int TamanhoP(Pilha *P) { return P->N; }
void DestroiP(Pilha *P) { while (TamanhoP(P) > 0) Desempilha(P); }

typedef struct Fila {
    No * Prox;
    No * Ult;
    int N;
} Fila;

void ConstroiF(Fila *F) { F->Prox = NULL; F->Ult = NULL; F->N = 0; }
void Enfileira(Fila *F, int x) {
    No * n = (No *) malloc(sizeof(No));
    n->E = x; n->Prox = NULL;
    if (F->Ult == NULL) { F->Prox = n; F->Ult = n; }
    else { F->Ult->Prox = n; F->Ult = n; }
    F->N++;
}
int Desenfileira(Fila *F) {
    No * n = F->Prox; int x = n->E;
    F->Prox = F->Prox->Prox;
    if (F->Prox == NULL) F->Ult = NULL;
    free(n); F->N--;
    return x;
}
int TamanhoF(Fila *F) { return F->N; }
void DestroiF(Fila *F) { while (TamanhoF(F) > 0) Desenfileira(F); }

int PilhasIdenticas(Pilha *p1, Pilha *p2) {
    if (TamanhoP(p1) != TamanhoP(p2)) return 0;
    Pilha a1, a2;
    ConstroiP(&a1); ConstroiP(&a2);
    int r = 1;
    while (TamanhoP(p1) > 0) {
        int v1 = Desempilha(p1);
        int v2 = Desempilha(p2);
        if (v1 != v2) r = 0;
        Empilha(&a1, v1);
        Empilha(&a2, v2);
    }
    while (TamanhoP(&a1) > 0) {
        Empilha(p1, Desempilha(&a1));
        Empilha(p2, Desempilha(&a2));
    }
    DestroiP(&a1); DestroiP(&a2);
    return r;
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    int n; Pilha p1; Pilha p2;
    while (scanf("%d", &n) > 0) {
        if (n != -1) {
            int e;
            ConstroiP(&p1); ConstroiP(&p2);
            for (int i = 0; i < n; i++) {
                scanf("%d", &e); Empilha(&p1, e);
            }
            scanf("%d", &n);
            for (int i = 0; i < n; i++) {
                scanf("%d", &e); Empilha(&p2, e);
            }
        } else {
            n = 1000000;
            ConstroiP(&p1); ConstroiP(&p2);
            for (int i = 0; i < n; i++) {
                Empilha(&p1, i+1); Empilha(&p2, i+1);
            }
        }
        printf("%d\n", PilhasIdenticas(&p1, &p2));
        DestroiP(&p1); DestroiP(&p2);
    }
    return 0;
}
