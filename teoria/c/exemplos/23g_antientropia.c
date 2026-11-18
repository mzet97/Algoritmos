/* Aula 23g — anti-entropia por resumos: sincronizar pelo que mudou.
 *
 * Dois nos com 64 chaves versionadas cada. Em vez de trocar os logs
 * inteiros (a 23c manda tudo), trocam 8 resumos FNV-1a (um por faixa de
 * 8 chaves): faixas iguais nem viajam; faixas divergentes trocam as 8
 * entradas. Tres cenarios deterministicos: lojas iguais (0 faixas),
 * uma chave divergente (1 faixa) e faixas espalhadas (sincroniza tudo
 * que diverge e nada que coincide).
 *
 * Compilar: gcc -std=c11 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/23g_antientropia.c -o /tmp/23g
 * Executar: /tmp/23g
 */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

enum { N_CHAVES = 64, FAIXAS = 8, POR_FAIXA = N_CHAVES / FAIXAS };

typedef struct {
    int presente;
    int valor;
    uint32_t versao;
} Entrada;

typedef struct {
    Entrada e[N_CHAVES];
} Loja;

static uint32_t fnv1a(const void *dados, size_t n)
{
    const unsigned char *p = dados;
    uint32_t h = 2166136261u;
    for (size_t i = 0; i < n; i++) {
        h ^= p[i];
        h *= 16777619u;
    }
    return h;
}

/* Resumo da faixa: FNV sobre (presente, valor, versao) das 8 entradas. */
static uint32_t resumo_faixa(const Loja *l, int f)
{
    unsigned char buf[POR_FAIXA * 12];
    size_t n = 0;
    for (int i = f * POR_FAIXA; i < (f + 1) * POR_FAIXA; i++) {
        memcpy(buf + n, &l->e[i].presente, 4);
        n += 4;
        memcpy(buf + n, &l->e[i].valor, 4);
        n += 4;
        memcpy(buf + n, &l->e[i].versao, 4);
        n += 4;
    }
    return fnv1a(buf, n);
}

static void loja_init(Loja *l)
{
    for (int i = 0; i < N_CHAVES; i++) {
        l->e[i].presente = 1;
        l->e[i].valor = i * 10;
        l->e[i].versao = 1;
    }
}

/* Sincroniza b a partir de a. Volta faixas trocadas e entradas movidas. */
static void antientropia(const Loja *a, Loja *b, int *faixas, int *entradas)
{
    *faixas = 0;
    *entradas = 0;
    for (int f = 0; f < FAIXAS; f++) {
        if (resumo_faixa(a, f) == resumo_faixa(b, f))
            continue; /* faixa igual: nem viaja */
        (*faixas)++;
        for (int i = f * POR_FAIXA; i < (f + 1) * POR_FAIXA; i++) {
            if (b->e[i].versao < a->e[i].versao) {
                b->e[i] = a->e[i];
                (*entradas)++;
            }
        }
    }
}

static int iguais(const Loja *a, const Loja *b)
{
    return memcmp(a, b, sizeof *a) == 0;
}

int main(void)
{
    Loja a, b;
    int faixas, entradas;

    /* Cenario 1: lojas iguais — zero faixas viajam. */
    loja_init(&a);
    loja_init(&b);
    antientropia(&a, &b, &faixas, &entradas);
    assert(faixas == 0 && entradas == 0 && iguais(&a, &b));
    printf("iguais: 0 faixas, 0 entradas (resumo decide, dado nem viaja)\n");

    /* Cenario 2: uma chave divergente — exatamente 1 faixa viaja. */
    b.e[40].valor = 9999;
    b.e[40].versao = 2;
    antientropia(&a, &b, &faixas, &entradas);
    assert(faixas == 1 && entradas == 0 && !iguais(&a, &b));
    printf("1 chave a mais em b: 1 faixa diverge, 0 entradas voltam (versao de b vence)\n");
    loja_init(&b);
    b.e[40].valor = 9999;
    b.e[40].versao = 2;
    antientropia(&b, &a, &faixas, &entradas);
    assert(faixas == 1 && entradas == 1 && iguais(&a, &b));
    assert(a.e[40].valor == 9999 && a.e[40].versao == 2);
    printf("reverso: 1 faixa, 1 entrada, lojas iguais (sincroniza o que diverge)\n");

    /* Cenario 3: divergencias espalhadas em 3 faixas — o resto nem viaja. */
    loja_init(&a);
    loja_init(&b);
    a.e[3].valor = 1;
    a.e[3].versao = 5; /* faixa 0 */
    a.e[30].valor = 2;
    a.e[30].versao = 5; /* faixa 3 */
    a.e[63].valor = 3;
    a.e[63].versao = 5; /* faixa 7 */
    antientropia(&a, &b, &faixas, &entradas);
    assert(faixas == 3 && entradas == 3 && iguais(&a, &b));
    printf("3 faixas divergentes: 3 faixas, 3 entradas, resto nem viajou\n");

    printf("23g ok: resumo decide, faixa igual nao viaja, divergente sincroniza\n");
    return 0;
}
