#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

enum { THREADS = 2, ITERATIONS = 100000 };

typedef struct {
    pthread_mutex_t mutex;
    int protected_count;
    atomic_int atomic_count;
} Counters;

static void *worker(void *arg) {
    Counters *c = arg;
    for (int i = 0; i < ITERATIONS; ++i) {
        if (pthread_mutex_lock(&c->mutex) != 0) abort();
        ++c->protected_count;
        if (pthread_mutex_unlock(&c->mutex) != 0) abort();
        atomic_fetch_add_explicit(&c->atomic_count, 1, memory_order_relaxed);
    }
    return NULL;
}

int main(void) {
    Counters c = { .protected_count = 0, .atomic_count = ATOMIC_VAR_INIT(0) };
    pthread_t threads[THREADS];
    if (pthread_mutex_init(&c.mutex, NULL) != 0) return 1;
    for (int i = 0; i < THREADS; ++i) {
        if (pthread_create(&threads[i], NULL, worker, &c) != 0) {
            fprintf(stderr, "pthread_create falhou\n");
            for (int j = 0; j < i; ++j) pthread_join(threads[j], NULL);
            pthread_mutex_destroy(&c.mutex);
            return 1;
        }
    }
    for (int i = 0; i < THREADS; ++i) {
        if (pthread_join(threads[i], NULL) != 0) abort();
    }
    printf("mutex=%d atomic=%d esperado=%d\n", c.protected_count,
           atomic_load_explicit(&c.atomic_count, memory_order_relaxed),
           THREADS * ITERATIONS);
    if (pthread_mutex_destroy(&c.mutex) != 0) return 1;
    return 0;
}
