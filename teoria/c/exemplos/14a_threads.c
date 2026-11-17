#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N_THREADS 8
#define N_REP 100000L

/* Contador com invariante: todo acesso sob mutex. */
typedef struct {
    pthread_mutex_t mu;
    long total;
} Contador;

static void *somar(void *arg) {
    Contador *c = arg;
    for (long i = 0; i < N_REP; i++) {
        pthread_mutex_lock(&c->mu);
        c->total++;
        pthread_mutex_unlock(&c->mu);
    }
    return NULL;
}

/* Handoff ordenado: produtor avisa, consumidor espera o predicado. */
typedef struct {
    pthread_mutex_t mu;
    pthread_cond_t cond;
    int pronto;
    int valor;
} Canal;

static void *produtor(void *arg) {
    Canal *c = arg;
    pthread_mutex_lock(&c->mu);
    c->valor = 42;
    c->pronto = 1;
    pthread_cond_signal(&c->cond);
    pthread_mutex_unlock(&c->mu);
    return NULL;
}

static void *consumidor(void *arg) {
    Canal *c = arg;
    pthread_mutex_lock(&c->mu);
    while (!c->pronto) {
        pthread_cond_wait(&c->cond, &c->mu);
    }
    int v = c->valor;
    pthread_mutex_unlock(&c->mu);
    /* Devolve pelo heap: sobrevive ao termino da thread. */
    int *saida = malloc(sizeof *saida);
    assert(saida != NULL);
    *saida = v;
    return saida;
}

int main(void) {
    /* 1. Contador: 8 x 100000, invariante exata. */
    Contador c;
    assert(pthread_mutex_init(&c.mu, NULL) == 0);
    c.total = 0;
    pthread_t ts[N_THREADS];
    for (int t = 0; t < N_THREADS; t++) {
        assert(pthread_create(&ts[t], NULL, somar, &c) == 0);
    }
    for (int t = 0; t < N_THREADS; t++) {
        assert(pthread_join(ts[t], NULL) == 0);
    }
    /* O &c acima e seguro: main aguarda todos os joins antes de sair. */
    assert(c.total == (long)N_THREADS * N_REP);
    printf("contador: %ld (esperado %ld)\n", c.total, (long)N_THREADS * N_REP);
    assert(pthread_mutex_destroy(&c.mu) == 0);

    /* 2. Handoff produtor -> consumidor com predicado em laco. */
    Canal canal;
    assert(pthread_mutex_init(&canal.mu, NULL) == 0);
    assert(pthread_cond_init(&canal.cond, NULL) == 0);
    canal.pronto = 0;
    canal.valor = 0;
    pthread_t prod, cons;
    assert(pthread_create(&cons, NULL, consumidor, &canal) == 0);
    assert(pthread_create(&prod, NULL, produtor, &canal) == 0);
    void *ret = NULL;
    assert(pthread_join(cons, &ret) == 0);
    assert(pthread_join(prod, NULL) == 0);
    assert(ret != NULL && *(int *)ret == 42);
    printf("handoff: valor = %d\n", *(int *)ret);
    free(ret);
    assert(pthread_mutex_destroy(&canal.mu) == 0);
    assert(pthread_cond_destroy(&canal.cond) == 0);

    printf("threads ok: invariantes preservadas\n");
    return EXIT_SUCCESS;
}
