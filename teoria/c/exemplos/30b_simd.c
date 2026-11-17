#define _GNU_SOURCE
#include <assert.h>
#include <math.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>

#if defined(__x86_64__) || defined(__i386__)
#include <immintrin.h>
#endif

static uint32_t lcg_estado = 7u;
static uint32_t lcg_proximo(void) {
    lcg_estado = lcg_estado * 1664525u + 1013904223u;
    return lcg_estado >> 8;
}

static double agora_s(void) {
    struct timespec ts;
    (void)clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

static float ponto_escalar(const float *a, const float *b, size_t n) {
    float s = 0.0f;
    for (size_t i = 0; i < n; i++) s += a[i] * b[i];
    return s;
}

/* Quatro acumuladores: quebra a cadeia de dependencia do somatorio.
   A ordem de soma muda, logo o resultado em float MUDA nos ultimos bits. */
static float ponto_4acum(const float *a, const float *b, size_t n) {
    float s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    size_t i = 0;
    for (; i + 4 <= n; i += 4) {
        s0 += a[i] * b[i];
        s1 += a[i + 1] * b[i + 1];
        s2 += a[i + 2] * b[i + 2];
        s3 += a[i + 3] * b[i + 3];
    }
    for (; i < n; i++) s0 += a[i] * b[i];
    return (s0 + s1) + (s2 + s3);
}

#if defined(__x86_64__) || defined(__i386__)
/* Compilado com atributo de alvo: o binario nasce generico e esta funcao
   so e chamada se a CPU anunciar AVX2+FMA em tempo de execucao. Sem o
   dispatch, um binario -march=native morreria com SIGILL numa CPU antiga. */
__attribute__((target("avx2,fma")))
static float ponto_avx(const float *a, const float *b, size_t n) {
    __m256 acc = _mm256_setzero_ps();
    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256 x = _mm256_loadu_ps(a + i);
        __m256 y = _mm256_loadu_ps(b + i);
        acc = _mm256_fmadd_ps(x, y, acc);
    }
    float lanes[8];
    _mm256_storeu_ps(lanes, acc);
    float s = ((lanes[0] + lanes[1]) + (lanes[2] + lanes[3])) +
              ((lanes[4] + lanes[5]) + (lanes[6] + lanes[7]));
    for (; i < n; i++) s += a[i] * b[i];
    return s;
}
#endif

static int contar_nos_numa(void) {
    DIR *d = opendir("/sys/devices/system/node");
    if (!d) return -1;
    int nos = 0;
    struct dirent *e;
    while ((e = readdir(d)) != NULL)
        if (strncmp(e->d_name, "node", 4) == 0 && e->d_name[4] >= '0' && e->d_name[4] <= '9') nos++;
    closedir(d);
    return nos;
}

int main(void) {
    /* Afinidade: quantas CPUs o SO nos deixa usar (cpuset de container conta). */
    cpu_set_t mascara;
    CPU_ZERO(&mascara);
    int cpus_disponiveis = -1;
    if (sched_getaffinity(0, sizeof mascara, &mascara) == 0)
        cpus_disponiveis = CPU_COUNT(&mascara);
    printf("cpus_disponiveis=%d nos_numa=%d\n", cpus_disponiveis, contar_nos_numa());
    assert(cpus_disponiveis >= 1);

    /* Fixar na primeira CPU disponivel e confirmar pela mascara relida. */
    int primeira = -1;
    for (int c = 0; c < CPU_SETSIZE; c++)
        if (CPU_ISSET(c, &mascara)) { primeira = c; break; }
    assert(primeira >= 0);
    cpu_set_t uma;
    CPU_ZERO(&uma);
    CPU_SET(primeira, &uma);
    if (sched_setaffinity(0, sizeof uma, &uma) == 0) {
        cpu_set_t conf;
        CPU_ZERO(&conf);
        assert(sched_getaffinity(0, sizeof conf, &conf) == 0);
        assert(CPU_COUNT(&conf) == 1 && CPU_ISSET(primeira, &conf));
        printf("afinidade: thread fixada na cpu %d e confirmado\n", primeira);
    } else {
        printf("afinidade: sched_setaffinity recusado neste ambiente (registrado)\n");
    }
    /* Restaura a mascara original para a medicao nao nascer enviesada. */
    (void)sched_setaffinity(0, sizeof mascara, &mascara);

    const size_t n = 10000000u;
    float *a = malloc(n * sizeof *a);
    float *b = malloc(n * sizeof *b);
    assert(a != NULL && b != NULL);
    for (size_t i = 0; i < n; i++) {
        a[i] = (float)(lcg_proximo() % 1000u) / 1000.0f;
        b[i] = (float)(lcg_proximo() % 1000u) / 1000.0f;
    }

    /* Oraculo em double: somar 10 milhoes de floats em sequencia perde
       ~1% para o arredondamento; 4 acumuladores perdem ~0.1%. Comparar
       float contra float com tolerancia apertada reprova quem esta CERTO. */
    double ref = 0.0;
    for (size_t i = 0; i < n; i++) ref += (double)a[i] * (double)b[i];
    float resc = ponto_escalar(a, b, n);
    float r4 = ponto_4acum(a, b, n);
    double e_esc = fabs((double)resc - ref) / ref;
    double e_4 = fabs((double)r4 - ref) / ref;
    printf("erro_vs_double: escalar=%.2e 4acum=%.2e\n", e_esc, e_4);
    assert(e_esc < 2e-2 && e_4 < 2e-3 && e_4 < e_esc);
#if defined(__x86_64__) || defined(__i386__)
    const int tem_avx = __builtin_cpu_supports("avx2") && __builtin_cpu_supports("fma");
    if (tem_avx) {
        float ravx = ponto_avx(a, b, n);
        assert(fabs((double)ravx - ref) / ref < 2e-3);
    }
#else
    const int tem_avx = 0;
#endif

    double t0 = agora_s();
    volatile float lixo = 0;
    for (int r = 0; r < 5; r++) lixo += ponto_escalar(a, b, n);
    double t_esc = (agora_s() - t0) / 5.0;
    t0 = agora_s();
    for (int r = 0; r < 5; r++) lixo += ponto_4acum(a, b, n);
    double t_4 = (agora_s() - t0) / 5.0;
    printf("escalar: %.4fs  4acum: %.4fs  razao: %.2fx\n", t_esc, t_4, t_esc / t_4);
#if defined(__x86_64__) || defined(__i386__)
    if (tem_avx) {
        t0 = agora_s();
        for (int r = 0; r < 5; r++) lixo += ponto_avx(a, b, n);
        double t_avx = (agora_s() - t0) / 5.0;
        printf("avx+fma: %.4fs  razao_vs_escalar: %.2fx\n", t_avx, t_esc / t_avx);
    } else {
        printf("avx: CPU nao anuncia avx2+fma; caminho generico usado\n");
    }
#else
    printf("avx: fora de x86; caminho generico usado\n");
#endif
    (void)lixo;
    free(a); free(b);
    printf("30b ok: corretude por tolerancia, afinidade confirmada, NUMA contado\n");
    return EXIT_SUCCESS;
}
