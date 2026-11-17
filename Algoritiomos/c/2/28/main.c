#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int E;
    struct No * Prox;
} No;

/* Helper: multiply polynomial P by scalar a */
No * ProdutoPolinomioEscalar(No * P, int a) {
    No * head = NULL;
    No * tail = NULL;
    No * p = P;
    while (p != NULL) {
        No * novo = (No *) malloc(sizeof(No));
        novo->E = p->E * a;
        novo->Prox = NULL;
        if (head == NULL) {
            head = novo;
        } else {
            tail->Prox = novo;
        }
        tail = novo;
        p = p->Prox;
    }
    return head;
}

/* Helper: add two polynomials */
No * SomaPolinomios(No * P, No * Q) {
    int nP = 0, nQ = 0;
    No * p = P;
    while (p != NULL) { nP++; p = p->Prox; }
    No * q = Q;
    while (q != NULL) { nQ++; q = q->Prox; }

    int maxN = nP > nQ ? nP : nQ;
    int * coeffs = (int *) calloc(maxN, sizeof(int));

    p = P;
    for (int i = nP - 1; i >= 0 && p != NULL; i--) {
        coeffs[maxN - nP + i] += p->E;
        p = p->Prox;
    }
    q = Q;
    for (int i = nQ - 1; i >= 0 && q != NULL; i--) {
        coeffs[maxN - nQ + i] += q->E;
        q = q->Prox;
    }

    No * head = NULL;
    No * tail = NULL;
    for (int i = 0; i < maxN; i++) {
        No * novo = (No *) malloc(sizeof(No));
        novo->E = coeffs[i];
        novo->Prox = NULL;
        if (head == NULL) {
            head = novo;
        } else {
            tail->Prox = novo;
        }
        tail = novo;
    }
    free(coeffs);
    return head;
}

void InsereFinalEnc(No ** L, int e, No ** u) {
    No * novo = (No *) malloc(sizeof(No));
    novo->E = e; novo->Prox = NULL;
    if (*u == NULL) {
        *L = novo;
    } else {
        (*u)->Prox = novo;
    }
    *u = novo;
}

void InsereInicioEnc(No ** L, int e) {
    No * novo = (No *) malloc(sizeof(No));
    novo->E = e; novo->Prox = *L; *L = novo;
}

void Destroi(No * L) {
    No * p, * pr; p = L;
    while (p != NULL) {
        pr = p; p = p->Prox;
        free(pr);
    }
}

No * ProdutoPolinomios(No * P, No * Q) {
    No * R = NULL; No * uR;
    No * p;

    p = P;

    R = NULL; uR = NULL; InsereFinalEnc(&R, 0, &uR);
    int zeros = 0;
    while (p != NULL) {
        No * T = ProdutoPolinomioEscalar(Q, p->E);
        for (int i=1; i<=zeros; i++) {
            InsereInicioEnc(&T, 0);
        }
        No * Rtemp = SomaPolinomios(R, T); Destroi(T); Destroi(R);
        R = Rtemp;
        zeros = zeros+1;
        p = p->Prox;
    }
    return R;
}

void Escreve(No * L) {
    printf("["); while (L != NULL) { printf("%d ", L->E); L = L->Prox; }; printf("]\n");
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    int n1, n2; No * P; No * Q; No * u; int e; No * R;
    while (scanf("%d %d", &n1, &n2)>0) {
        P = NULL; Q = NULL;
        if (n1!=-1) {
            u = NULL;
            for (int i=0; i<=n1; i++) {
                scanf("%d", &e);
                InsereFinalEnc(&P,e,&u);
            }
            u = NULL;
            for (int i=0; i<=n2; i++) {
                scanf("%d", &e);
                InsereFinalEnc(&Q,e,&u);
            }
        } else {
            n1 = 1000; n2 = 1000;
            u = NULL;
            for (int i=0; i<=n1; i++) {
                InsereFinalEnc(&P,1,&u);
            }
            u = NULL;
            for (int i=0; i<=n2; i++) {
                InsereFinalEnc(&Q,1,&u);
            }
        }
        R = ProdutoPolinomios(P, Q);
        Escreve(R); Destroi(R); Destroi(P); Destroi(Q);
    }
    return 0;
}
