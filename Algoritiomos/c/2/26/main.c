#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int E;
    struct No * Prox;
} No;

/* Add two polynomials represented as linked lists of coefficients.
   P = a_n, a_(n-1), ..., a_0  (highest degree first)
   Q = b_m, b_(m-1), ..., b_0
   Result: coefficient-by-coefficient addition.
   If degrees differ, the shorter polynomial is padded with leading zeros. */
No * SomaPolinomios(No * P, No * Q) {
    /* Determine lengths */
    int nP = 0, nQ = 0;
    No * p = P;
    while (p != NULL) { nP++; p = p->Prox; }
    No * q = Q;
    while (q != NULL) { nQ++; q = q->Prox; }

    /* Create result array, then build list */
    int maxN = nP > nQ ? nP : nQ;
    int * coeffs = (int *) calloc(maxN, sizeof(int));

    /* Fill from the end (a_0 is the last element) */
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

    /* Build result list */
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

void InsereEnc(No ** L, int e, No ** u) {
    No * novo = (No *) malloc(sizeof(No));
    novo->E = e; novo->Prox = NULL;
    if (*u == NULL) {
        *L = novo;
    } else {
        (*u)->Prox = novo;
    }
    *u = novo;
}

void Destroi(No * L) {
    No * p, * pr; p = L;
    while (p != NULL) {
        pr = p; p = p->Prox;
        free(pr);
    }
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
                InsereEnc(&P,e,&u);
            }
            u = NULL;
            for (int i=0; i<=n2; i++) {
                scanf("%d", &e);
                InsereEnc(&Q,e,&u);
            }
        } else {
            n1 = 1000000; n2 = 1000000;
            u = NULL;
            InsereEnc(&P,0,&u);
            for (int i=0; i<n1; i++) {
                InsereEnc(&P,1,&u);
            }
            u = NULL;
            InsereEnc(&Q,1,&u);
            for (int i=0; i<n2; i++) {
                InsereEnc(&Q,-1,&u);
            }
        }
        R = SomaPolinomios(P, Q);
        Escreve(R); Destroi(R); Destroi(P); Destroi(Q);
    }
    return 0;
}
