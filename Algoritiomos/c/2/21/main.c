#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int E;
    struct No * Prox;
} No;

typedef struct ListaLinear {
    No * Inicio;
    int N;
} ListaLinear;

/* Difference: elements in L1 that are NOT in L2.
   Both lists are sorted. */
ListaLinear Diferenca(ListaLinear L1, ListaLinear L2) {
    ListaLinear result;
    result.Inicio = NULL;
    result.N = 0;
    No * tail = NULL;

    No * p1 = L1.Inicio;
    No * p2 = L2.Inicio;

    while (p1 != NULL) {
        /* Advance p2 until p2->E >= p1->E or p2 is NULL */
        while (p2 != NULL && p2->E < p1->E) {
            p2 = p2->Prox;
        }
        /* Check if p1->E is in L2 */
        int found = 0;
        if (p2 != NULL && p2->E == p1->E) {
            found = 1;
        }
        if (!found) {
            No * novo = (No *) malloc(sizeof(No));
            novo->E = p1->E;
            novo->Prox = NULL;
            if (result.Inicio == NULL) {
                result.Inicio = novo;
            } else {
                tail->Prox = novo;
            }
            tail = novo;
            result.N++;
        }
        p1 = p1->Prox;
    }
    return result;
}

void Insere(ListaLinear *L, int e, No ** u) {
    No * novo = (No *) malloc(sizeof(No));
    novo->E = e; novo->Prox = NULL;
    if (*u == NULL) {
        L->Inicio = novo;
    } else {
        (*u)->Prox = novo;
    }
    *u = novo; L->N = L->N + 1;
}

int cmp_int(const void *a, const void *b) {
    return (*(int*)a) - (*(int*)b);
}

void Escreve(ListaLinear L) {
    int n = L.N;
    No * p = L.Inicio;
    int * Lv = (int *) malloc(sizeof(int)*n);
    int i = 0;
    while (p != NULL) {
        Lv[i] = p->E; i++; p = p->Prox;
    }
    qsort(Lv, n, sizeof(int), cmp_int);
    printf("["); for (int i=0; i<n; i++) printf("%d ", Lv[i]); printf("]\n");
    free(Lv);
}

void Destroi(ListaLinear L) {
    No * p, * pr; p = L.Inicio;
    while (p != NULL) {
        pr = p; p = p->Prox;
        free(pr);
    }
}

ListaLinear C_Diferenca(ListaLinear L1, ListaLinear L2) {
#ifndef CODEV
    printf("CODEV_BEGIN_EXEC\n");
#endif
    ListaLinear r;
    r = Diferenca(L1,L2);
#ifndef CODEV
    printf("CODEV_END_EXEC\n");
#endif
    return r;
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    int max_n1, max_n2; ListaLinear L1; ListaLinear L2; ListaLinear L; No * u;
    while (scanf("%d %d", &max_n1, &max_n2)>0) {
        if (max_n1!=-1) {
            L1.Inicio = NULL; L1.N = 0; u = NULL;
            for (int i=0; i<max_n1; i++) {
                int e; scanf("%d", &e); Insere(&L1,e,&u);
            }
            L2.Inicio = NULL; L2.N = 0; u = NULL;
            for (int i=0; i<max_n2; i++) {
                int e; scanf("%d", &e); Insere(&L2,e,&u);
            }
            L = C_Diferenca(L1,L2); Escreve(L);
            Destroi(L1); Destroi(L2); Destroi(L);
        } else {
            max_n1 = 9000; max_n2 = max_n1;
            L1.Inicio = NULL; L1.N = 0; u = NULL;
            for (int i=0; i<max_n1; i++) {
                int e; Insere(&L1,i*2,&u);
            }
            L2.Inicio = NULL; L2.N = 0; u = NULL;
            for (int i=0; i<max_n2; i++) {
                int e; Insere(&L2,i*3,&u);
            }
            L = C_Diferenca(L1,L2); Escreve(L);
            Destroi(L1); Destroi(L2); Destroi(L);
        }
    }
    return 0;
}
