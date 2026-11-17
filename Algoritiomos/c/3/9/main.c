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

int SomaSequenciaOperacoes(int S[], int n) {
    Pilha P; ConstroiP(&P);
    int soma = 0;
    for (int i = 0; i < n; i++) {
        if (S[i] == 0) {
            if (TamanhoP(&P) > 0) {
                soma -= Desempilha(&P);
            }
        } else {
            Empilha(&P, S[i]);
            soma += S[i];
        }
    }
    DestroiP(&P);
    return soma;
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    int n; int * S;
    while (scanf("%d", &n) > 0) {
        if (n != -1) {
            S = (int *) malloc(sizeof(int)*n);
            for (int i = 0; i < n; i++) {
                scanf("%d", &S[i]);
            }
        } else {
            n = 1000000;
            S = (int *) malloc(sizeof(int)*n);
            for (int i = 0; i < n/2; i++) {
                S[i] = 1; S[n/2+i] = 0;
            }
        }
        printf("%d\n", SomaSequenciaOperacoes(S, n));
        free(S);
    }
    return 0;
}
