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

typedef struct Fila {
    No * Prox;
    No * Ult;
    int N;
} Fila;

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

void LerEscreverKUltimos(int k) {
    Fila F; ConstroiF(&F);
    int x;
    while (scanf("%d", &x) > 0 && x != 0) {
        Enfileira(&F, x);
        if (TamanhoF(&F) > k) {
            Desenfileira(&F);
        }
    }
    while (TamanhoF(&F) > 0) {
        printf("%d ", Desenfileira(&F));
    }
    printf("\n");
    DestroiF(&F);
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    int k;
    while (scanf("%d", &k) > 0) {
        LerEscreverKUltimos(k);
    }
    return 0;
}
