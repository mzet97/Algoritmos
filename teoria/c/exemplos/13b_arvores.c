#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* LCG deterministico (mesma familia da aula 13a): semente fixa, sem rand(). */
static uint32_t lcg_estado = 1234u;
static uint32_t lcg_proximo(void) {
    lcg_estado = lcg_estado * 1664525u + 1013904223u;
    return lcg_estado >> 8;
}

/* ---------- BST nao balanceada: o invariante e a ordem, nao a forma ---------- */
typedef struct No {
    int chave;
    struct No *esq, *dir;
} No;

static No *no_novo(int chave) {
    No *n = malloc(sizeof *n);
    if (!n) return NULL;
    n->chave = chave;
    n->esq = n->dir = NULL;
    return n;
}

/* Devolve 1 se inseriu, 0 se a chave ja existia, -1 em falha de alocacao. */
static int bst_inserir(No **raiz, int chave) {
    while (*raiz) {
        if (chave == (*raiz)->chave) return 0;
        raiz = chave < (*raiz)->chave ? &(*raiz)->esq : &(*raiz)->dir;
    }
    *raiz = no_novo(chave);
    return *raiz ? 1 : -1;
}

static int bst_contem(const No *raiz, int chave) {
    while (raiz) {
        if (chave == raiz->chave) return 1;
        raiz = chave < raiz->chave ? raiz->esq : raiz->dir;
    }
    return 0;
}

static No *bst_minimo(No *raiz) {
    while (raiz && raiz->esq) raiz = raiz->esq;
    return raiz;
}

/* Remocao: folha e um filho sao splices; dois filhos copiam o sucessor
   (minimo da direita) e removem o sucessor, que tem no maximo um filho. */
static int bst_remover(No **raiz, int chave) {
    if (!*raiz) return 0;
    if (chave < (*raiz)->chave) return bst_remover(&(*raiz)->esq, chave);
    if (chave > (*raiz)->chave) return bst_remover(&(*raiz)->dir, chave);
    No *vitima = *raiz;
    if (!vitima->esq) { *raiz = vitima->dir; free(vitima); return 1; }
    if (!vitima->dir) { *raiz = vitima->esq; free(vitima); return 1; }
    No *suc = bst_minimo(vitima->dir);
    vitima->chave = suc->chave;
    return bst_remover(&vitima->dir, suc->chave);
}

static size_t bst_altura(const No *raiz) {
    if (!raiz) return 0;
    size_t e = bst_altura(raiz->esq), d = bst_altura(raiz->dir);
    return 1 + (e > d ? e : d);
}

static void bst_em_ordem(const No *raiz, int *saida, size_t *n) {
    if (!raiz) return;
    bst_em_ordem(raiz->esq, saida, n);
    saida[(*n)++] = raiz->chave;
    bst_em_ordem(raiz->dir, saida, n);
}

static void bst_destruir(No *raiz) {
    if (!raiz) return;
    bst_destruir(raiz->esq);
    bst_destruir(raiz->dir);
    free(raiz);
}

/* ---------- mergesort estavel com buffer auxiliar ---------- */
static void mesclar(int *v, int *aux, size_t ini, size_t meio, size_t fim) {
    size_t i = ini, j = meio, k = ini;
    while (i < meio && j < fim)
        aux[k++] = v[i] <= v[j] ? v[i++] : v[j++]; /* <= : estavel */
    while (i < meio) aux[k++] = v[i++];
    while (j < fim) aux[k++] = v[j++];
    memcpy(v + ini, aux + ini, (fim - ini) * sizeof *v);
}

static void mergesort_rec(int *v, int *aux, size_t ini, size_t fim) {
    if (fim - ini < 2) return;
    size_t meio = ini + (fim - ini) / 2;
    mergesort_rec(v, aux, ini, meio);
    mergesort_rec(v, aux, meio, fim);
    mesclar(v, aux, ini, meio, fim);
}

/* Devolve 1 em sucesso, 0 em falha de alocacao (vetor intacto). */
static int mergesort(int *v, size_t n) {
    if (n < 2) return 1;
    if (n > SIZE_MAX / sizeof(int)) return 0;
    int *aux = malloc(n * sizeof *aux);
    if (!aux) return 0;
    mergesort_rec(v, aux, 0, n);
    free(aux);
    return 1;
}

static int comparar_int(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}

int main(void) {
    /* BST: inserir, duplicata recusada, buscar, remover os tres casos. */
    No *t = NULL;
    const int chaves[] = {50, 30, 70, 20, 40, 60, 80, 65};
    for (size_t i = 0; i < sizeof chaves / sizeof *chaves; i++)
        assert(bst_inserir(&t, chaves[i]) == 1);
    assert(bst_inserir(&t, 40) == 0);          /* duplicata: sem efeito */
    assert(bst_contem(t, 65) && !bst_contem(t, 66));
    assert(bst_remover(&t, 20) == 1);          /* folha */
    assert(bst_remover(&t, 60) == 1);          /* um filho (65) */
    assert(bst_remover(&t, 50) == 1);          /* dois filhos: sucessor */
    assert(bst_remover(&t, 999) == 0);         /* ausente */
    int ord[8]; size_t n_ord = 0;
    bst_em_ordem(t, ord, &n_ord);
    const int esperado[] = {30, 40, 65, 70, 80};
    assert(n_ord == 5 && memcmp(ord, esperado, sizeof esperado) == 0);
    bst_destruir(t);

    /* Propriedade: em-ordem da BST == qsort do mesmo multiconjunto. */
    for (int teste = 0; teste < 200; teste++) {
        size_t n = (size_t)(lcg_proximo() % 60u);
        int *v = malloc((n ? n : 1) * sizeof *v);
        assert(v != NULL);
        No *arv = NULL;
        size_t distintas = 0;
        for (size_t i = 0; i < n; i++) {
            v[i] = (int)(lcg_proximo() % 40u) - 20;
            int r = bst_inserir(&arv, v[i]);
            assert(r >= 0);
            if (r == 1) distintas++;
        }
        qsort(v, n, sizeof *v, comparar_int);
        /* Remove duplicatas do oraculo para comparar com a BST (conjunto). */
        size_t m = 0;
        for (size_t i = 0; i < n; i++)
            if (i == 0 || v[i] != v[i - 1]) v[m++] = v[i];
        int *saida = malloc((distintas ? distintas : 1) * sizeof *saida);
        assert(saida != NULL);
        size_t k = 0;
        bst_em_ordem(arv, saida, &k);
        assert(k == distintas && k == m);
        assert(memcmp(saida, v, m * sizeof *v) == 0);
        free(saida);
        bst_destruir(arv);
        free(v);
    }

    /* Degeneracao: inserir ordenado transforma a BST em lista. */
    No *lista = NULL, *embaralhada = NULL;
    for (int i = 0; i < 1000; i++) assert(bst_inserir(&lista, i) == 1);
    assert(bst_altura(lista) == 1000);
    /* Mesma faixa, ordem do LCG (Fisher-Yates deterministico). */
    int perm[1000];
    for (int i = 0; i < 1000; i++) perm[i] = i;
    for (int i = 999; i > 0; i--) {
        int j = (int)(lcg_proximo() % (uint32_t)(i + 1));
        int tmp = perm[i]; perm[i] = perm[j]; perm[j] = tmp;
    }
    for (int i = 0; i < 1000; i++) assert(bst_inserir(&embaralhada, perm[i]) == 1);
    size_t h = bst_altura(embaralhada);
    printf("altura ordenada=1000 embaralhada=%zu (esperado ~log2(1000)=10)\n", h);
    assert(h < 100); /* degenerada seria 1000; embaralhada fica perto de log */
    bst_destruir(lista);
    bst_destruir(embaralhada);

    /* Mergesort contra qsort como oraculo, com duplicatas. */
    for (int teste = 0; teste < 500; teste++) {
        size_t n = (size_t)(lcg_proximo() % 80u);
        int *a = malloc((n ? n : 1) * sizeof *a);
        int *b = malloc((n ? n : 1) * sizeof *b);
        assert(a != NULL && b != NULL);
        for (size_t i = 0; i < n; i++) a[i] = b[i] = (int)(lcg_proximo() % 61u) - 30;
        assert(mergesort(a, n) == 1);
        qsort(b, n, sizeof *b, comparar_int);
        assert(memcmp(a, b, n * sizeof *a) == 0);
        free(a); free(b);
    }
    printf("13b ok: BST (3 casos de remocao), degeneracao medida, mergesort==qsort\n");
    return EXIT_SUCCESS;
}
