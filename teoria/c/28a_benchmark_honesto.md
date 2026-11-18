---
layout: default
---

# Aula 28a — Benchmark honesto: escala, falso compartilhamento e o modelo de custo

**Competências SDD:** HPC, GPU e SEC. **Pré-requisitos:** aulas 14a, 17 e 28. Você precisa saber threads, invariantes, `clock_gettime` e o que é uma linha de cache. Não precisa saber SIMD, MPI, GPU nem Assembly.

**Objetivo observável:** ao terminar, você deverá medir uma soma particionada em 1 contra N threads com correção verificada antes do cronômetro, demonstrar falso compartilhamento com e sem preenchimento, e decidir no papel se um acelerador hipotético compensa — com a conta de transferência explícita.

**Motivação:** benchmarks mentirosos são fáceis: medir código errado, cronometrar uma vez, ignorar a variância, esconder o ambiente e declarar vitória. Esta aula define o protocolo honesto — hipótese, baseline correto, aquecimento, repetições, dispersão, ambiente — e o aplica a dois fenômenos reais: escala limitada por banda e falso compartilhamento.

**Como ler esta aula (três níveis):** *fundamental* — protocolo honesto e os dois fenômenos medidos; *intermediário* — dispersão, ambiente e a conta da transferência; *aprofundamento* — o acelerador hipotético decidido no papel.

## 1. O protocolo, antes do cronômetro

1. **Hipótese escrita:** "somar em N threads é mais rápido que em 1 porque..." — se você não sabe por que deveria ser, a medição não ensina nada.
2. **Correção primeiro:** baseline sequencial verificado contra fórmula fechada; cada variante paralela afirma igualdade com o baseline.
3. **Aquecimento:** primeira execução fora da conta (páginas, caches, turbo, governador).
4. **Repetições e dispersão:** execute 3+ vezes, reporte todas, use o *mínimo* como número principal (o mínimo filtra ruído para baixo; a média esconde bimodalidade).
5. **Ambiente anotado:** CPU, núcleos, SO, compilador, flags, carga. Sem isso, o número não é reproduzível — é anedota.

## 2. Escala: largura de banda, não mágica

Somar 100 milhões de `int64` é *streaming*: o gargalo é trazer bytes da RAM, não somar. Quatro núcleos pedindo bytes do mesmo barramento não andam 4x mais rápido — a hipótese honesta prevê ganho modesto, e a medição confirma. Paralelismo ajuda quando o trabalho é computacional por byte, não quando é fome de banda.

## 3. Falso compartilhamento: vizinhos que brigam

Oito contadores em posições adjacentes (`long v[8]`) dividem 1–2 linhas de cache de 64 bytes. Cada thread escreve "sua" variável, mas o hardware invalida a linha inteira a cada escrita alheia: o programa está correto e lento. Com `alignas(64)` por contador, cada um ganha sua linha e a briga some. Mesma aritmética, ordens de magnitude diferentes — e o `assert` de correção passa nos dois, porque correção nunca foi o problema.

## 4. O modelo de custo do acelerador (sem placa)

Antes de pedir GPU, faça a conta no papel: `tempo_total = transferência_ida + computação_acelerada + transferência_volta`. Se a transferência domina, nem aceleração infinita salva — é a lei de Amdahl com pedágio. O exemplo desta aula mede a parte que roda na CPU e deixa a fórmula pronta: com os números medidos, calcule qual aceleração mínima justificaria mover os dados.

## 5. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/28a_benchmark.c) contém exatamente o programa abaixo (`-pthread`, `-O2` para medir código otimizado de verdade):

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -O2 -pthread teoria/c/exemplos/28a_benchmark.c -o bench28a
./bench28a
```

```c
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
```

Saída esperada (números e razão variam por máquina, carga e turbo):

```text
seq=~s par4=~s speedup=~x
vizinhos=~s isolados=~s razao=~x
benchmark ok: correcao antes, numeros depois
```

Leia com o modelo certo: espero speedup modesto na soma (streaming limitado por banda, não por CPU) e razão grande em vizinhos/isolados (invalidação de linha contra independência). Se a sua máquina mostrar outra coisa, a hipótese escrita antes da medição é o que torna o resultado interessante em vez de confuso: anote o ambiente e investigue (turbo, afinidade, carga, NUMA).

## 6. Erros comuns e implicações defensivas

1. **Medir uma vez:** turbo e carga dançam. Repita e reporte o mínimo com a dispersão.
2. **Medir código errado:** sem `assert` de igualdade com o baseline, o benchmark pode estar cronometrando o bug.
3. **Medir sem `-O2`:** depuração desliga otimização e vetorigação; o número não representa produção.
4. **Confundir contenção com paralelismo:** some partições independentes; some contadores compartilhados só para provar o custo.
5. **Alegar ganho de GPU sem a conta de transferência:** `t_total = t_ida + t_kernel + t_volta`. Sem ela, é marketing.
6. **Benchmark que nunca falha:** se o teste passa até com código errado, ele não testa nada. Toda invariante aqui falha se a soma estiver errada.

## 7. Ponte com as demais trilhas

Python mede o mesmo tradeoff entre threads e processos (aula 16a); Go mede worker pool e race detector (aula 11a); Rust mede localidade sequencial contra salto (aula 16a); C++ mede mutex contra atômico (aula 17a). O protocolo desta aula — hipótese, baseline, aquecimento, repetições, ambiente — vale para todas.

---

### Pesos da avaliacao

| Criterio | Peso |
|---|---|
| Compreensao conceitual (previsao, diagnostico, explicacao) | 40% |
| Analise de compilacao e ferramentas | 25% |
| Funcionamento (compila sem avisos e roda) | 20% |
| Documentacao (comandos, versao, saidas) | 15% |

A execucao bem-sucedida sozinha nao aprova: a maior parcela esta em demonstrar que voce entendeu o porque.

### Para se aprofundar

1. Randall Bryant e David O'Hallaron, *Computer Systems: A Programmer's Perspective* (Pearson, 3. ed., ISBN 978-0134092669), caps. 5-6; 2. Agner Fog, manuais de microarquitetura ([agner.org/optimize](https://www.agner.org/optimize/)); 3. John Patterson e John Hennessy, *Computer Organization and Design, RISC-V Edition* (Morgan Kaufmann, ISBN 978-0128203316).
