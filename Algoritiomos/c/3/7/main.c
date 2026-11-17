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
int ProximoF(Fila *F) { return F->Prox->E; }
int TamanhoF(Fila *F) { return F->N; }
void DestroiF(Fila *F) { while (TamanhoF(F) > 0) Desenfileira(F); }

int FilasIdenticas(Fila *f1, Fila *f2) {
    if (TamanhoF(f1) != TamanhoF(f2)) return 0;
    Fila c1, c2;
    ConstroiF(&c1); ConstroiF(&c2);
    int r = 1;
    while (TamanhoF(f1) > 0) {
        int a = Desenfileira(f1);
        int b = Desenfileira(f2);
        if (a != b) r = 0;
        Enfileira(&c1, a);
        Enfileira(&c2, b);
    }
    while (TamanhoF(&c1) > 0) {
        Enfileira(f1, Desenfileira(&c1));
        Enfileira(f2, Desenfileira(&c2));
    }
    DestroiF(&c1); DestroiF(&c2);
    return r;
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    int n; Fila f1; Fila f2;
    while (scanf("%d", &n) > 0) {
        if (n != -1) {
            int e;
            ConstroiF(&f1); ConstroiF(&f2);
            for (int i = 0; i < n; i++) {
                scanf("%d", &e); Enfileira(&f1, e);
            }
            scanf("%d", &n);
            for (int i = 0; i < n; i++) {
                scanf("%d", &e); Enfileira(&f2, e);
            }
        } else {
            int e; n = 1000000;
            ConstroiF(&f1); ConstroiF(&f2);
            for (int i = 0; i < n; i++) {
                Enfileira(&f1, i+1); Enfileira(&f2, i+1);
            }
        }
        printf("%d\n", FilasIdenticas(&f1, &f2));
        DestroiF(&f1); DestroiF(&f2);
    }
    return 0;
}
