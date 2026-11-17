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

void Constroi(Pilha *P) {
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

int Topo(Pilha *P) {
    return P->Topo->E;
}

int Tamanho(Pilha *P) {
    return P->N;
}

void Ordena(Pilha *P) {
    Pilha Q; Constroi(&Q);
    while (Tamanho(P) > 0) {
        int tmp = Desempilha(P);
        while (Tamanho(&Q) > 0 && Topo(&Q) > tmp) {
            Empilha(P, Desempilha(&Q));
        }
        Empilha(&Q, tmp);
    }
    while (Tamanho(&Q) > 0) {
        Empilha(P, Desempilha(&Q));
    }
}

void Escreve(Pilha *P) {
    while (Tamanho(P) > 0) {
        printf("%d ", Desempilha(P));
    }
    printf("\n");
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    Pilha P; Constroi(&P);
    int n; int e;
    while (scanf("%d", &n) > 0) {
        if (n >= 0) {
            for (int i = 1; i <= n; i++) {
                scanf("%d", &e); Empilha(&P, e);
            }
            Ordena(&P); Escreve(&P);
        } else {
            if (n == -1) {
                for (int i = 1; i <= 1000; i++) {
                    Empilha(&P, i + (1001)*(i%2));
                }
            } else {
                for (int i = 1; i <= 1000; i++) {
                    Empilha(&P, i + (1001)*(i%5 == 0 ? 0 : 1));
                }
            }
            Ordena(&P); Escreve(&P);
        }
    }
    return 0;
}
