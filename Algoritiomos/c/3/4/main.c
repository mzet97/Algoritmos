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

int TamanhoP(Pilha *P) { return P->N; }

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

void ConstroiP(Pilha *P) { P->Topo = NULL; P->N = 0; }

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

typedef struct Parentese {
    char desc;
    int tipo;
} Parentese;

int VerificaParentizacao(Parentese E[], int n) {
    Pilha P; ConstroiP(&P);
    for (int i = 0; i < n; i++) {
        if (E[i].desc == '(') {
            Empilha(&P, E[i].tipo);
        } else {
            if (TamanhoP(&P) == 0) { DestroiP(&P); return 0; }
            int tp = Desempilha(&P);
            if (tp != E[i].tipo) { DestroiP(&P); return 0; }
        }
    }
    int r = (TamanhoP(&P) == 0) ? 1 : 0;
    DestroiP(&P);
    return r;
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    Parentese * E; int n;
    while (scanf("%d", &n) > 0) {
        if (n >= 0) {
            char * Ec = (char *) malloc(sizeof(char)*4*n);
            E = (Parentese *) malloc(sizeof(Parentese)*n);
            scanf("%s", Ec);
            int j = 0;
            for (int i = 0; i < n; i++) {
                E[i].desc = Ec[j]; j++;
                char tipo[10];
                int z = j;
                while ((Ec[j] != '\0') && (Ec[j] != '(') && (Ec[j] != ')')) {
                    tipo[j-z] = Ec[j];
                    j = j+1;
                }
                tipo[j-z] = '\0';
                E[i].tipo = atoi(tipo);
            }
            printf("%d\n", VerificaParentizacao(E, n));
            free(E); free(Ec);
        } else {
            int nlin = n;
            n = 1000000;
            E = (Parentese *) malloc(sizeof(Parentese)*n);
            if (nlin == -1) {
                for (int i = 0; i < n/2; i++) { E[i].desc = '('; E[i].tipo = 1; }
                for (int i = n/2; i < n; i++) { E[i].desc = ')'; E[i].tipo = 1; }
            } else if (nlin == -2) {
                for (int i = 0; i < n/2; i++) { E[i].desc = '('; E[i].tipo = i+1; }
                for (int i = n/2; i < n; i++) { E[i].desc = ')'; E[i].tipo = n-(i+1)+1; }
            } else if (nlin == -3) {
                for (int i = 0; i < n; i += 2) { E[i].desc = '('; E[i].tipo = i+1; E[i+1].desc = ')'; E[i+1].tipo = i+1; }
            } else if (nlin == -4) {
                for (int i = 0; i < n/2-1; i++) { E[i].desc = '('; E[i].tipo = 1; }
                for (int i = n/2-1; i < n; i++) { E[i].desc = ')'; E[i].tipo = 1; }
            } else if (nlin == -5) {
                for (int i = 0; i < n/2+1; i++) { E[i].desc = '('; E[i].tipo = 1; }
                for (int i = n/2+1; i < n; i++) { E[i].desc = ')'; E[i].tipo = 1; }
            }
            printf("%d\n", VerificaParentizacao(E, n));
            free(E);
        }
    }
    return 0;
}
