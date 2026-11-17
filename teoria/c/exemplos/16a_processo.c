#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/* Cronometra um milhao de getpid: régua do custo minimo de syscall. */
static double medir_getpid(int repeticoes) {
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int i = 0; i < repeticoes; i++) {
        volatile pid_t p = getpid();
        (void)p;
    }
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double ini = (double)t0.tv_sec + (double)t0.tv_nsec / 1e9;
    double fim = (double)t1.tv_sec + (double)t1.tv_nsec / 1e9;
    return (fim - ini) / repeticoes;
}

/* Conta quantas entradas do mapa sao legiveis e executaveis. */
static void resumir_mapas(int *total, int *exec) {
    FILE *f = fopen("/proc/self/maps", "r");
    assert(f != NULL);
    char linha[512];
    *total = 0;
    *exec = 0;
    while (fgets(linha, sizeof linha, f) != NULL) {
        (*total)++;
        if (strstr(linha, "r-xp") != NULL) {
            (*exec)++;
        }
    }
    fclose(f);
}

int main(void) {
    /* 1. Processo filho com codigo de saida conhecido. */
    pid_t filho = fork();
    assert(filho >= 0);
    if (filho == 0) {
        _exit(7);
    }
    int estado = 0;
    assert(waitpid(filho, &estado, 0) == filho);
    assert(WIFEXITED(estado) && WEXITSTATUS(estado) == 7);
    printf("filho colhido: codigo=%d\n", WEXITSTATUS(estado));

    /* 2. Régua de syscall. */
    const int rep = 1000000;
    printf("getpid: %.1f ns/chamada (%d repeticoes)\n",
           medir_getpid(rep) * 1e9, rep);

    /* 3. Mapa de memoria do proprio processo. */
    int total = 0, exec = 0;
    resumir_mapas(&total, &exec);
    printf("regioes mapeadas: %d, executaveis: %d\n", total, exec);
    assert(total > 0 && exec > 0);

    /* 4. Faltas de pagina: menores (reclamadas sem I/O) x maiores (com I/O). */
    struct rusage antes, depois;
    assert(getrusage(RUSAGE_SELF, &antes) == 0);
    const size_t n = 8u * 1024u * 1024u;
    unsigned char *bloco = malloc(n);
    assert(bloco != NULL);
    for (size_t i = 0; i < n; i += 4096) {
        bloco[i] = 1; /* toca uma pagina por vez: falta menor por pagina */
    }
    assert(getrusage(RUSAGE_SELF, &depois) == 0);
    printf("faltas menores: %ld -> %ld\n",
           antes.ru_minflt, depois.ru_minflt);
    printf("faltas maiores: %ld -> %ld\n",
           antes.ru_majflt, depois.ru_majflt);
    assert(depois.ru_minflt > antes.ru_minflt);
    free(bloco);

    /* 5. Entrada nao confiavel: fork de comando inexistente falha no filho. */
    pid_t f2 = fork();
    assert(f2 >= 0);
    if (f2 == 0) {
        execl("/nao/existe/de/jeito/nenhum", "x", (char *)NULL);
        _exit(127); /* execl so retorna em erro */
    }
    int e2 = 0;
    assert(waitpid(f2, &e2, 0) == f2);
    assert(WIFEXITED(e2) && WEXITSTATUS(e2) == 127);
    printf("exec falho contido: codigo=%d\n", WEXITSTATUS(e2));

    return EXIT_SUCCESS;
}
