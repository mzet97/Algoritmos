#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int E;
    struct No *Prox;
} No;

void RemoveMaiores(No **L, int k) {
    while (*L != NULL && (*L)->E > k) {
        No *temp = *L;
        *L = (*L)->Prox;
        free(temp);
    }
    if (*L == NULL) return;
    No *p = *L;
    while (p->Prox != NULL) {
        if (p->Prox->E > k) {
            No *temp = p->Prox;
            p->Prox = temp->Prox;
            free(temp);
        } else {
            p = p->Prox;
        }
    }
}

int cmp_int(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
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

void Insere(No **L, int e) {
    No *novo = (No *)malloc(sizeof(No));
    novo->E = e;
    novo->Prox = *L;
    *L = novo;
}

void C_RemoveMaiores_wrapper(No **L, int k, int q) {
    for (int t = 0; t < q; t++) {
        RemoveMaiores(L, k);
    }
}

int main() {
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
    int max_n;
    No *L = NULL;
    while (scanf("%d", &max_n) > 0) {
        if (max_n != -1) {
            for (int i = 0; i < max_n; i++) {
                int e;
                scanf("%d", &e);
                Insere(&L, e);
            }
            int k;
            scanf("%d", &k);
            C_RemoveMaiores_wrapper(&L, k, 1);
            Escreve(L);
            Destroi(&L);
        } else {
            max_n = 1000000;
            for (int i = 0; i < max_n; i++) {
                Insere(&L, (i % 2 == 0 ? 2 : 3));
            }
            C_RemoveMaiores_wrapper(&L, 2, 1);
            Escreve(L);
            Destroi(&L);
        }
    }
    return 0;
}
