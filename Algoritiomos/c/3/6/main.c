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

void ManobrarVagoes(int n, int V[]) {
    Pilha P; ConstroiP(&P);
    Fila F; ConstroiF(&F);
    int esperado = 1;
    int idx = 0;
    while (esperado <= n) {
        if (TamanhoP(&P) > 0 && TopoP(&P) == esperado) {
            Enfileira(&F, Desempilha(&P));
            esperado++;
        } else if (idx < n) {
            Empilha(&P, V[idx++]);
        } else {
            break;
        }
    }
    while (TamanhoF(&F) > 0) {
        printf("%d ", Desenfileira(&F));
    }
    printf("\n");
    DestroiP(&P);
    DestroiF(&F);
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    int n; int * V;
    while (scanf("%d", &n) > 0) {
        if (n != -1) {
            V = (int *) malloc(sizeof(int) * n);
            for (int i = 0; i < n; i++) {
                scanf("%d", &V[i]);
            }
        } else {
            n = 1000;
            V = (int *) malloc(sizeof(int) * n);
            int M = n; int m = 1;
            for (int i = n-1; i >= 0; i = i-2) {
                V[i] = M; V[i-1] = m;
                M = M-1; m = m+1;
            }
        }
        ManobrarVagoes(n, V);
        free(V);
    }
    return 0;
}
