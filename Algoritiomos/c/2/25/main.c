#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int E;
    struct No * Prox;
} No;

/* Evaluate polynomial at x using Horner's method.
   P = a_n * x^n + a_(n-1) * x^(n-1) + ... + a_0
   Horner: start from a_n, multiply by x and add next coefficient.
   List stores: a_n, a_(n-1), ..., a_0 */
long long int AvaliaPolinomio(No * P, int x) {
    long long int result = 0;
    No * p = P;
    while (p != NULL) {
        result = result * x + p->E;
        p = p->Prox;
    }
    return result;
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

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    int n; No * P; No * u; int e; int x;
    while (scanf("%d", &n)>0) {
        P = NULL; u = NULL;
        if (n!=-1) {
            for (int i=0; i<=n; i++) {
                scanf("%d", &e);
                InsereEnc(&P,e,&u);
            }
            scanf("%d", &x);
        } else {
            n = 10000000;
            for (int i=0; i<=n; i++) {
                InsereEnc(&P,1,&u);
            }
            x = 1;
        }
        printf("%lld\n", AvaliaPolinomio(P, x));
        Destroi(P);
    }
    return 0;
}
