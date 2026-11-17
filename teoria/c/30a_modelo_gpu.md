---
layout: default
---

# Aula 30a — Modelo de execução em GPU, sem placa: SIMT, hierarquia e a conta da transferência

**Competências SDD:** HPC e GPU. **Pré-requisitos:** aulas 17, 28, 28a e 30. Você precisa saber medir com baseline verificado, o que é localidade e como ler um speedup sem se iludir. Não precisa de placa GPU, CUDA, driver proprietário nem cluster.

**Objetivo observável:** ao terminar, você deverá explicar o modelo de execução SIMT (grade, bloco, thread), a hierarquia de memória do acelerador e por que a transferência host↔dispositivo decide sozinha se vale a pena — e demonstrar cada ideia com um análogo executável em CPU comum.

**Motivação:** GPU sem placa parece contradição, mas o contrato editorial exige exatamente isso no núcleo obrigatório: modelo de execução, transferências, hierarquia de memória, granularidade e critério de ganho — com implementação de referência em CPU. A placa física fica para o laboratório opcional; quem entende o modelo na CPU chega ao laboratório sabendo o que medir.

**Limite explícito:** nada aqui executa em GPU. Números de GPU citados como motivação são ordem de grandeza pública, não promessa para nenhum modelo. O que é medido neste livro roda em CPU e está marcado como tal.

## 1. O modelo: grade, bloco, thread

Um kernel GPU organiza o trabalho em três níveis:

- **thread:** a unidade mínima, executa a mesma instrução que suas vizinhas (SIMT: *single instruction, multiple threads*);
- **bloco:** grupo de threads que cooperam via memória compartilhada rápida e sincronizam entre si;
- **grade:** o conjunto de blocos que cobre o problema inteiro; blocos escalam com o hardware (mais SMs, mais blocos simultâneos).

O análogo em CPU: um laço aninhado onde o interno é o "bloco" (faixa contígua que cabe em cache) e a iteração é a "thread". A diferença fundamental: na CPU, threads divergentes custam pouco; no SIMT, threads do mesmo grupo (*warp*) que tomam ramos diferentes **serializam** — todas pagam os dois lados do `if`.

## 2. A hierarquia decide antes da aritmética

Do mais rápido e escasso ao mais lento e abundante (ordens de grandeza típicas):

- **registradores** por thread: zero latência extra, mas poucos por thread — estourar vira *spill* para memória lenta;
- **memória compartilhada** por bloco: rápida, explícita, pequena — o programador move os dados para lá de propósito;
- **memória global**: grande, lenta, coalescida quando threads vizinhas leem endereços vizinhos; acesso espalhado desperdiça a maior parte de cada transação.

O experimento desta aula demonstra o análogo em CPU: multiplicação ingênua (padrão de acesso hostil ao cache) contra multiplicação por blocos (faixas que ficam residentes — o "shared memory" do pobre). Mesma aritmética, tempos muito diferentes.

## 3. A conta da viagem decide tudo

Antes de qualquer kernel, some a viagem:

```text
t_total = bytes_ida / banda + t_kernel + bytes_volta / banda
```

Se a transferência domina, nem kernel infinitamente rápido salva: com 800 MB e 16 GB/s efetivos, cada sentido custa ~0,05 s. Um kernel de CPU de 0,2 s exige do acelerador menos de 0,1 s *no total* para compensar. Abaixo desse limiar, a resposta honesta é "não compensa mover" — e essa resposta vale mais que qualquer benchmark.

## 4. Divergência: o `if` que custa em grupo

Em CPU com predição de ramos, dados ordenados voam e dados aleatórios tropeçam. No SIMT o efeito é estrutural: o grupo executa os dois caminhos. O exemplo mede o primo pobre desse fenômeno em CPU — seleção com ramo contra seleção sem ramo, em dados ordenados e embaralhados (LCG com semente fixa) — para treinar o olho: padrão de dados previsível é desempenho, não detalhe.

## 5. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/30a_modelo_gpu.c) contém exatamente o programa abaixo (CPU comum, `-O2`):

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -O2 teoria/c/exemplos/30a_modelo_gpu.c -o modelo30a
./modelo30a
```

```c
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
```

Saída esperada (tempos e razões variam por máquina, turbo e carga):

```text
correcao: ingenuo == blocos (N=384)
ingenuo=~s blocos=~s razao=~x (esperado: blocos mais rapido, 2x ou mais)
ramo ordenado=~s ramo embaralhado=~s sem-ramo=~s
soma=... (ancora contra otimizacao)
viagem=0.093s kernel_cpu=0.200s orcamento_kernel=0.107s
modelo ok: referencia CPU, conta da viagem e criterio
```

Leia com o modelo certo: o tiled vence porque as faixas BxB moram em cache durante o reuso — o análogo honesto de "shared memory" sem alegar hardware que não foi medido. A conta da viagem é aritmética pura com asserts: 800 MiB a 16 GiB/s por sentido dá ~0,047 s por sentido, ~0,093 s ida-volta. Se a sua máquina medir outra banda, troque o número e refaça a conta — o método é o entregável, não o dígito.

### O que a memória faz aqui?

O ingênuo percorre `B` por colunas com passo de `n` doubles: cada acesso paga uma linha de cache quase inteira desperdiçada. O tiled reordena para `a * B[k][j]` com `j` sequencial dentro do bloco: localidade espacial e temporal juntas. Os vetores estáticos evitam estouro de pilha (384² doubles ≈ 1,1 MiB cada). O `volatile soma` ancora o benchmark contra eliminação por otimizador esperto demais.

## 6. Erros comuns e implicações defensivas

1. **Medir sem baseline verificado:** asserts de igualdade primeiro, cronômetro depois. Sempre.
2. **Comparar O2 contra O0:** o flag muda tudo. Declare-o junto do número.
3. **Alegar "GPU acelera Nx" sem a viagem:** some ida e volta antes do verbo.
4. **Confundir tiled com "otimização mágica":** é localidade, mensurável e explicável linha a linha.
5. **Benchmark que nunca falha:** se a conta da viagem estiver errada e nada acusar, o teste é teatro. Os asserts numéricos prendem a aritmética.
6. **Prometer placa que não mediu:** hardware específico mora no laboratório opcional, com versão de driver e modelo anotados.

## 7. Ponte com as demais trilhas

Python mediria o mesmo tradeoff entre NumPy e laço puro (aula 29a quando existir); Go, entre baseline e multicore (G7); Rust, escalar contra multicore antes de qualquer `wgpu` (R7). O modelo — grade, bloco, transferência, granularidade — é o mesmo em todas; só a placa muda.
