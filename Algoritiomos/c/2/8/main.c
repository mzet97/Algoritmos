#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int E;
    struct No *Prox;
} No;

No *SemRepeticoes(No *L) {
    /* Build a new list with unique elements from L.
       Use a simple O(n^2) approach since elements are integers.
       For each element, check if it already appeared before. */
    No *result = NULL;
    No *tail = NULL;
    No *p = L;
    while (p != NULL) {
        /* Check if p->E already exists in result */
        int found = 0;
        No *q = result;
        while (q != NULL) {
            if (q->E == p->E) {
                found = 1;
                break;
            }
            q = q->Prox;
        }
        if (!found) {
            No *novo = (No *)malloc(sizeof(No));
            novo->E = p->E;
            novo->Prox = NULL;
            if (result == NULL) {
                result = novo;
            } else {
                tail->Prox = novo;
            }
            tail = novo;
        }
        p = p->Prox;
    }
    return result;
}

int cmp_int(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

void Insere(No **L, int e) {
    No *novo = (No *)malloc(sizeof(No));
    novo->E = e;
    novo->Prox = *L;
    *L = novo;
}

void Escreve(No *L) {
    int n = 0;
    No *p = L;
    while (p != NULL) {
        n++;
        p = p->Prox;
    }
    int *Lv = (int *)malloc(sizeof(int) * n);
    p = L;
    int i = 0;
    while (p != NULL) {
        Lv[i] = p->E;
        i++;
        p = p->Prox;
    }
    qsort(Lv, n, sizeof(int), cmp_int);
    printf("[");
    for (int i = 0; i < n; i++) printf("%d", Lv[i]);
    printf("]\n");
    free(Lv);
}

void Destroi(No **L) {
    No *p, *pr;
    p = *L;
    while (p != NULL) {
        pr = p;
        p = p->Prox;
        free(pr);
    }
    *L = NULL;
}

No *C_SemRepeticoes(No *L, int q) {
    No *r;
    for (int t = 0; t < q; t++) {
        r = SemRepeticoes(L);
    }
    return r;
}

int main() {
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
    int n;
    No *L = NULL;
    No *Lr;
    while (scanf("%d", &n) > 0) {
        if (n > -1) {
            for (int i = 0; i < n; i++) {
                int e;
                scanf("%d", &e);
                Insere(&L, e);
            }
            Lr = C_SemRepeticoes(L, 1);
            Escreve(Lr);
            Destroi(&Lr);
            Destroi(&L);
        } else {
            int nlin = n;
            n = 1000;
            for (int i = 0; i < n; i++) {
                Insere(&L, (i % 2 == 0 ? 2 : 3));
            }
            if (nlin != -1) {
                for (int i = 0; i < n; i++) {
                    Insere(&L, i);
                }
            }
            Lr = C_SemRepeticoes(L, 1);
            Escreve(Lr);
            Destroi(&Lr);
            Destroi(&L);
        }
    }
    return 0;
}
