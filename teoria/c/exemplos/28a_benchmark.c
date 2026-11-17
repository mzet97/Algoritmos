#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <pthread.h>
#include <stdalign.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N (100000000L)
#define N_THREADS 4
#define N_REP 3

static double agora_s(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

/* Soma [ini, fim) com invariante: acumulado == soma dos ja visitados. */
static int64_t somar_faixa(const int64_t *v, long ini, long fim) {
    int64_t s = 0;
    for (long i = ini; i < fim; i++) {
        s += v[i];
    }
    return s;
}

typedef struct {
    const int64_t *v;
    long ini, fim;
    int64_t parcial;
} Tarefa;

static void *trabalhar(void *arg) {
    Tarefa *t = arg;
    t->parcial = somar_faixa(t->v, t->ini, t->fim);
    return NULL;
}

/* Contadores vizinhos: dividem linhas de cache. */
typedef struct {
    volatile long v[N_THREADS];
} Vizinhos;

/* Contadores isolados: um por linha de 64 bytes. */
typedef struct {
    alignas(64) volatile long v;
    char preenche[64 - sizeof(long)];
} Isolado;

typedef struct {
    Vizinhos *viz;
    Isolado *iso;
    int id;
    long repeticoes;
    int modo; /* 0 = vizinhos, 1 = isolados */
} IncTarefa;

static void *incrementar(void *arg) {
    IncTarefa *t = arg;
    if (t->modo == 0) {
        for (long i = 0; i < t->repeticoes; i++) {
            t->viz->v[t->id]++;
        }
    } else {
        for (long i = 0; i < t->repeticoes; i++) {
            t->iso[t->id].v++;
        }
    }
    return NULL;
}

int main(void) {
    int64_t *v = malloc((size_t)N * sizeof *v);
    assert(v != NULL);
    for (long i = 0; i < N; i++) {
        v[i] = i % 97;
    }

    /* Baseline correto primeiro: soma direta confere o particionado. */
    int64_t esperado = 0;
    for (long i = 0; i < N; i++) {
        esperado += v[i];
    }
    int64_t seq = somar_faixa(v, 0, N);
    assert(seq == esperado);

    /* Aquecimento fora da conta. */
    (void)somar_faixa(v, 0, N);

    /* 1. Escala 1 x N_THREADS, minimo de N_REP rodadas. */
    double melhor_seq = 1e30, melhor_par = 1e30;
    for (int r = 0; r < N_REP; r++) {
        double t0 = agora_s();
        volatile int64_t s = somar_faixa(v, 0, N);
        (void)s;
        double dt = agora_s() - t0;
        if (dt < melhor_seq) {
            melhor_seq = dt;
        }

        Tarefa ts[N_THREADS];
        pthread_t ths[N_THREADS];
        t0 = agora_s();
        for (int w = 0; w < N_THREADS; w++) {
            ts[w] = (Tarefa){v, N * w / N_THREADS, N * (w + 1) / N_THREADS, 0};
            assert(pthread_create(&ths[w], NULL, trabalhar, &ts[w]) == 0);
        }
        int64_t soma = 0;
        for (int w = 0; w < N_THREADS; w++) {
            assert(pthread_join(ths[w], NULL) == 0);
            soma += ts[w].parcial;
        }
        dt = agora_s() - t0;
        if (dt < melhor_par) {
            melhor_par = dt;
        }
        assert(soma == esperado);
    }
    printf("seq=%.3fs par%d=%.3fs speedup=%.2fx\n",
           melhor_seq, N_THREADS, melhor_par, melhor_seq / melhor_par);

    /* 2. Falso compartilhamento: vizinhos contra isolados. */
    const long rep = 20000000L;
    Vizinhos vz = {{0}};
    double t0 = agora_s();
    pthread_t ths[N_THREADS];
    IncTarefa inc[N_THREADS];
    for (int w = 0; w < N_THREADS; w++) {
        inc[w] = (IncTarefa){&vz, NULL, w, rep, 0};
        assert(pthread_create(&ths[w], NULL, incrementar, &inc[w]) == 0);
    }
    for (int w = 0; w < N_THREADS; w++) {
        assert(pthread_join(ths[w], NULL) == 0);
    }
    double t_viz = agora_s() - t0;
    for (int w = 0; w < N_THREADS; w++) {
        assert(vz.v[w] == rep);
    }

    Isolado iso[N_THREADS];
    for (int w = 0; w < N_THREADS; w++) {
        iso[w].v = 0;
    }
    t0 = agora_s();
    for (int w = 0; w < N_THREADS; w++) {
        inc[w] = (IncTarefa){NULL, iso, w, rep, 1};
        assert(pthread_create(&ths[w], NULL, incrementar, &inc[w]) == 0);
    }
    for (int w = 0; w < N_THREADS; w++) {
        assert(pthread_join(ths[w], NULL) == 0);
    }
    double t_iso = agora_s() - t0;
    for (int w = 0; w < N_THREADS; w++) {
        assert(iso[w].v == rep);
    }
    printf("vizinhos=%.3fs isolados=%.3fs razao=%.2fx\n",
           t_viz, t_iso, t_viz / t_iso);

    printf("benchmark ok: correcao antes, numeros depois\n");
    free(v);
    return EXIT_SUCCESS;
}
