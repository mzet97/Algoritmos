#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAB_CAP 64u

/* LCG deterministico: sem dependencia externa, semente fixa. */
static uint32_t lcg_estado = 42u;

static uint32_t lcg_proximo(void) {
    lcg_estado = lcg_estado * 1664525u + 1013904223u;
    return lcg_estado >> 8;
}

/* Busca binaria: pre-condicao vetor ordenado em ordem nao decrescente. */
static long busca_binaria(const int *v, size_t n, int alvo) {
    size_t ini = 0;
    size_t fim = n;
    while (ini < fim) {
        /* ini + (fim - ini) / 2 nunca soma dois grandes: sem overflow. */
        size_t meio = ini + (fim - ini) / 2;
        if (v[meio] == alvo) {
            return (long)meio;
        }
        if (alvo < v[meio]) {
            fim = meio;
        } else {
            ini = meio + 1;
        }
    }
    return -1L;
}

/* Tabela hash didatica: sondagem linear, capacidade fixa, sem remocao. */
typedef struct {
    unsigned int chave;
    int valor;
    int ocupado; /* 0 = livre, 1 = ocupado */
} Slot;

typedef struct {
    Slot slots[TAB_CAP];
    size_t usadas;
} Tabela;

static void tabela_iniciar(Tabela *t) {
    memset(t, 0, sizeof *t);
}

/* Devolve 1 e grava, ou 0 se a tabela estiver cheia (fracasso explicito). */
static int tabela_inserir(Tabela *t, unsigned int chave, int valor) {
    if (t->usadas >= TAB_CAP) {
        return 0;
    }
    size_t pos = (size_t)chave % TAB_CAP;
    for (size_t i = 0; i < TAB_CAP; i++) {
        size_t s = (pos + i) % TAB_CAP;
        if (!t->slots[s].ocupado || t->slots[s].chave == chave) {
            if (!t->slots[s].ocupado) {
                t->usadas++;
            }
            t->slots[s].chave = chave;
            t->slots[s].valor = valor;
            t->slots[s].ocupado = 1;
            return 1;
        }
    }
    return 0;
}

static int tabela_buscar(const Tabela *t, unsigned int chave, int *saida) {
    size_t pos = (size_t)chave % TAB_CAP;
    for (size_t i = 0; i < TAB_CAP; i++) {
        size_t s = (pos + i) % TAB_CAP;
        if (!t->slots[s].ocupado) {
            return 0;
        }
        if (t->slots[s].chave == chave) {
            *saida = t->slots[s].valor;
            return 1;
        }
    }
    return 0;
}

static int comparar_int(const void *a, const void *b) {
    int x = *(const int *)a;
    int y = *(const int *)b;
    return (x > y) - (x < y);
}

int main(void) {
    assert(busca_binaria(NULL, 0, 5) == -1L);
    int um[] = {5};
    assert(busca_binaria(um, 1, 5) == 0L);
    assert(busca_binaria(um, 1, 4) == -1L);

    /* Propriedade: ordena copia com qsort e confere busca em tudo. */
    for (int t = 0; t < 500; t++) {
        size_t n = (size_t)(lcg_proximo() % 51u);
        /* Guarda contra overflow da conta de alocacao: n * sizeof(int). */
        if (n > SIZE_MAX / sizeof(int)) {
            fputs("tamanho grande demais\n", stderr);
            return EXIT_FAILURE;
        }
        int *v = malloc(n * sizeof *v);
        if (n > 0 && v == NULL) {
            fputs("falha ao alocar\n", stderr);
            return EXIT_FAILURE;
        }
        for (size_t i = 0; i < n; i++) {
            v[i] = (int)(lcg_proximo() % 201u) - 100;
        }
        qsort(v, n, sizeof *v, comparar_int);
        for (size_t k = 0; k < n; k++) {
            assert(busca_binaria(v, n, v[k]) >= 0L);
        }
        assert(busca_binaria(v, n, 1000) == -1L);
        assert(busca_binaria(v, n, -1000) == -1L);
        free(v);
    }

    /* Hash: insere 0..63 (enche), a 64a recusa; tudo inserido e achavel. */
    Tabela tab;
    tabela_iniciar(&tab);
    for (unsigned int k = 0; k < TAB_CAP; k++) {
        assert(tabela_inserir(&tab, k, (int)(k * 10u)));
    }
    assert(tabela_inserir(&tab, TAB_CAP, 1) == 0);
    for (unsigned int k = 0; k < TAB_CAP; k++) {
        int achado = 0;
        assert(tabela_buscar(&tab, k, &achado) && achado == (int)(k * 10u));
    }
    int lixo = 0;
    assert(!tabela_buscar(&tab, TAB_CAP + 1000u, &lixo));

    /* Medicao: linear x binaria sobre um milhao de elementos ordenados. */
    const size_t grande_n = 1000000u;
    int *grande = malloc(grande_n * sizeof *grande);
    assert(grande != NULL);
    for (size_t i = 0; i < grande_n; i++) {
        grande[i] = (int)(i * 2u);
    }
    clock_t t0 = clock();
    volatile long r_lin = -2L;
    for (int r = 0; r < 5; r++) {
        for (size_t i = 0; i < grande_n; i++) {
            if (grande[i] == 1999999) {
                r_lin = (long)i;
                break;
            }
        }
        r_lin = -1L; /* ausente: percorre tudo, o pior caso */
    }
    double t_lin = (double)(clock() - t0) / CLOCKS_PER_SEC / 5.0;
    t0 = clock();
    volatile long r_bin = -2L;
    for (int r = 0; r < 200; r++) {
        r_bin = busca_binaria(grande, grande_n, 1999999);
    }
    double t_bin = (double)(clock() - t0) / CLOCKS_PER_SEC / 200.0;
    assert(r_lin == -1L && r_bin == -1L);
    printf("linear: %.6fs/media (5x)\n", t_lin);
    printf("binaria: %.9fs/media (200x)\n", t_bin);
    free(grande);
    return EXIT_SUCCESS;
}
