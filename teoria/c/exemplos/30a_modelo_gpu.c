#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 384
#define BLOCO 32
#define NREP 3

static double agora_s(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

/* Referencia ingenua: i-j-k, acesso hostil a B por colunas. */
static void matmul_ingenuo(const double *A, const double *B, double *C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double soma = 0.0;
            for (int k = 0; k < n; k++) {
                soma += A[i * n + k] * B[k * n + j];
            }
            C[i * n + j] = soma;
        }
    }
}

/* Por blocos: faixas BxB residentes — o "shared memory" do pobre. */
static void matmul_blocos(const double *A, const double *B, double *C, int n) {
    memset(C, 0, (size_t)n * (size_t)n * sizeof *C);
    for (int ii = 0; ii < n; ii += BLOCO) {
        for (int jj = 0; jj < n; jj += BLOCO) {
            for (int kk = 0; kk < n; kk += BLOCO) {
                for (int i = ii; i < ii + BLOCO; i++) {
                    for (int k = kk; k < kk + BLOCO; k++) {
                        double a = A[i * n + k];
                        for (int j = jj; j < jj + BLOCO; j++) {
                            C[i * n + j] += a * B[k * n + j];
                        }
                    }
                }
            }
        }
    }
}

/* LCG deterministico para embaralhar sem biblioteca externa. */
static uint32_t lcg_estado = 12345u;

static uint32_t lcg_proximo(void) {
    lcg_estado = lcg_estado * 1664525u + 1013904223u;
    return lcg_estado >> 8;
}

int main(void) {
    /* 1. Correcao primeiro: ingênuo contra blocos na mesma entrada. */
    static double A[(size_t)N * N], B[(size_t)N * N];
    static double C1[(size_t)N * N], C2[(size_t)N * N];
    for (int i = 0; i < N * N; i++) {
        A[i] = (double)(i % 13) * 0.5;
        B[i] = (double)(i % 7) * 0.25;
    }
    matmul_ingenuo(A, B, C1, N);
    matmul_blocos(A, B, C2, N);
    for (int i = 0; i < N * N; i++) {
        double d = C1[i] - C2[i];
        assert(d > -1e-9 && d < 1e-9);
    }
    printf("correcao: ingenuo == blocos (N=%d)\n", N);

    /* Aquecimento fora da conta. */
    matmul_blocos(A, B, C2, N);

    /* 2. Medicao: minimo de NREP rodadas por variante. */
    double t_ing = 1e30, t_blk = 1e30;
    for (int r = 0; r < NREP; r++) {
        double t0 = agora_s();
        matmul_ingenuo(A, B, C1, N);
        double dt = agora_s() - t0;
        if (dt < t_ing) {
            t_ing = dt;
        }
        t0 = agora_s();
        matmul_blocos(A, B, C2, N);
        dt = agora_s() - t0;
        if (dt < t_blk) {
            t_blk = dt;
        }
    }
    printf("ingenuo=%.3fs blocos=%.3fs razao=%.2fx\n", t_ing, t_blk, t_ing / t_blk);

    /* 3. Ramo contra sem-ramo, ordenado contra embaralhado. */
    const int M = 8000000;
    int *ordenado = malloc((size_t)M * sizeof *ordenado);
    int *mexido = malloc((size_t)M * sizeof *mexido);
    assert(ordenado != NULL && mexido != NULL);
    for (int i = 0; i < M; i++) {
        ordenado[i] = i;
        mexido[i] = i;
    }
    for (int i = M - 1; i > 0; i--) {
        int j = (int)(lcg_proximo() % (unsigned)(i + 1));
        int tmp = mexido[i];
        mexido[i] = mexido[j];
        mexido[j] = tmp;
    }
    volatile long soma = 0;
    double t0 = agora_s();
    for (int i = 0; i < M; i++) {
        if (ordenado[i] < M / 2) {
            soma += ordenado[i];
        }
    }
    double t_ord_ramo = agora_s() - t0;
    t0 = agora_s();
    for (int i = 0; i < M; i++) {
        if (mexido[i] < M / 2) {
            soma += mexido[i];
        }
    }
    double t_mex_ramo = agora_s() - t0;
    t0 = agora_s();
    for (int i = 0; i < M; i++) {
        soma += mexido[i] < M / 2 ? mexido[i] : 0;
    }
    double t_mex_plano = agora_s() - t0;
    printf("ramo ordenado=%.3fs ramo embaralhado=%.3fs sem-ramo=%.3fs\n",
           t_ord_ramo, t_mex_ramo, t_mex_plano);
    printf("soma=%ld (ancora contra otimizacao)\n", (long)soma);
    free(ordenado);
    free(mexido);

    /* 4. Conta da viagem: aritmetica deterministica, sem hardware. */
    const double bytes = 800.0 * 1024.0 * 1024.0;
    const double banda_gbs = 16.0;
    const double t_kernel_cpu = 0.2;
    double t_viagem = 2.0 * (bytes / (banda_gbs * 1024.0 * 1024.0 * 1024.0));
    double teto_kernel = t_kernel_cpu - t_viagem;
    printf("viagem=%.3fs kernel_cpu=%.3fs orcamento_kernel=%.3fs\n",
           t_viagem, t_kernel_cpu, teto_kernel);
    assert(t_viagem > 0.09 && t_viagem < 0.11);
    assert(teto_kernel > 0.09 && teto_kernel < 0.11);
    printf("modelo ok: referencia CPU, conta da viagem e criterio\n");
    return EXIT_SUCCESS;
}
