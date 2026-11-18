/* Aula 30c — NUMA: onde a memoria mora e por que isso importa.
 *
 * Deteccao honesta + medicao local: conta os nos em
 * /sys/devices/system/node, mede banda de um buffer e explica first-touch
 * vs interleave — declarando que esta maquina tem 1 no, entao a comparacao
 * entre nos fica como protocolo, nao como medicao.
 *
 * Compilar: gcc -std=c11 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/30c_numa.c -o /tmp/30c
 * Executar: /tmp/30c
 */
#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int conta_nos(void)
{
    int n = 0;
    DIR *d = opendir("/sys/devices/system/node");
    if (d == NULL) {
        return -1;
    }
    struct dirent *e;
    while ((e = readdir(d)) != NULL) {
        if (strncmp(e->d_name, "node", 4) == 0) {
            n++;
        }
    }
    closedir(d);
    return n;
}

static double agora_s(void)
{
    struct timespec t;
    assert(clock_gettime(CLOCK_MONOTONIC, &t) == 0);
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

int main(void)
{
    int nos = conta_nos();
    printf("numa: nos detectados em /sys = %d\n", nos);
    assert(nos >= 1);

    /* Banda local: o numero que importa e a ordem de grandeza, nao o digito. */
    const size_t n = (size_t)64 << 20; /* 64 MiB */
    unsigned char *buf = malloc(n);
    assert(buf != NULL);
    memset(buf, 0x5a, n); /* first-touch: quem toca primeiro ancora a pagina */
    double ini = agora_s();
    unsigned long soma = 0;
    for (size_t passada = 0; passada < 4; passada++) {
        for (size_t i = 0; i < n; i += 64) {
            soma += buf[i];
        }
    }
    double fim = agora_s();
    double gbps = (4.0 * (double)n / (1 << 30)) / (fim - ini);
    printf("numa: varredura 4x64MiB em %.2fs = %.1f GiB/s (soma=%lu)\n",
           fim - ini, gbps, soma);
    free(buf);

    if (nos == 1) {
        printf("numa: 1 no nesta maquina — first-touch vs interleave sem efeito\n"
               "mensuravel aqui; o protocolo abaixo vale quando houver 2+ nos:\n"
               "  numactl --cpunodebind=0 --membind=0 ./prog   (local)\n"
               "  numactl --cpunodebind=0 --membind=1 ./prog (remoto)\n"
               "  numactl --interleave=all ./prog              (espalhado)\n"
               "30c ok: nos contados, banda local medida, limite multinó declarado\n");
    } else {
        printf("numa: %d nos — rode o protocolo acima e compare local x remoto\n"
               "30c ok: nos contados, banda local medida\n",
               nos);
    }
    return 0;
}
