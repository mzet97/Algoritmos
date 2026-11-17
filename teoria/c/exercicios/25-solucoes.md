---
layout: default
---

# Aula 25: solucoes comentadas

Compare apos tentar os [exercicios](../25.md#20-exercicios-obrigatorios).

1 e 2. Arena com alinhamento de 8 e pool com contadores, no mesmo programa:

```c
#include <stdio.h>
#include <stdlib.h>

#define ARENA_CAP 1024

struct Arena {
    unsigned char memoria[ARENA_CAP];
    size_t usadas;
};

static void *arena_aloca(struct Arena *a, size_t n)
{
    size_t alinhado = (n + 7u) & ~7u; /* proximo multiplo de 8 */
    if (alinhado > ARENA_CAP - a->usadas) {
        return NULL;
    }
    void *p = a->memoria + a->usadas;
    a->usadas += alinhado;
    return p;
}

static void arena_reseta(struct Arena *a)
{
    a->usadas = 0;
}

#define POOL_CAP 4

struct Celula {
    int valor;
    int em_uso;
};

struct Pool {
    struct Celula celulas[POOL_CAP];
    size_t hits;
    size_t misses;
};

static struct Celula *pool_aloca(struct Pool *p)
{
    for (size_t i = 0; i < POOL_CAP; i++) {
        if (!p->celulas[i].em_uso) {
            p->celulas[i].em_uso = 1;
            p->hits++;
            return &p->celulas[i];
        }
    }
    p->misses++;
    return NULL;
}

static void pool_libera(struct Pool *p, struct Celula *c)
{
    (void)p;
    c->em_uso = 0;
}

int main(void)
{
    struct Arena a = {{0}, 0};
    int *x = arena_aloca(&a, 5);  /* consome 8 */
    char *y = arena_aloca(&a, 13); /* consome 16 */
    if (x == NULL || y == NULL) {
        return 1;
    }
    *x = 42;
    y[0] = 'z';
    printf("arena: x=%d y=%c usadas=%zu\n", *x, y[0], a.usadas);
    arena_reseta(&a);
    printf("arena apos reset: usadas=%zu\n", a.usadas);

    struct Pool p = {{{0, 0}}, 0, 0};
    struct Celula *guardadas[POOL_CAP];
    for (size_t i = 0; i < POOL_CAP; i++) {
        guardadas[i] = pool_aloca(&p);
    }
    struct Celula *extra = pool_aloca(&p); /* miss: pool cheio */
    pool_libera(&p, guardadas[0]);
    struct Celula *reuso = pool_aloca(&p); /* hit: celula liberada */
    printf("pool: hits=%zu misses=%zu extra=%s reuso=%s\n",
           p.hits, p.misses, extra == NULL ? "null" : "ok",
           reuso != NULL ? "ok" : "null");
    return 0;
}
```

Saida esperada: `arena: x=42 y=z usadas=24` (5→8 e 13→16), reset volta a zero sem custo por objeto; `pool: hits=5 misses=1`. O alinhamento custa no maximo 7 bytes por alocacao — preco conhecido e limitado, pago para que qualquer objeto de ate 8 bytes de alinhamento caiba no ponteiro devolvido neste laboratorio (tipos com alinhamento maior exigiriam alinhar pelo tipo, nao por constante).

3. Num parser de requests, quase tudo o que a fase de parse aloca (tokens, nos da arvore, strings temporarias) nasce e morre com o request: arena vence, porque liberar e um reset O(1) e nao ha fragmentacao entre requests. Pool vence quando os objetos tem tamanho uniforme e vidas individuais — conexoes, buffers de rede reutilizados milhares de vezes.

4. O risco da arena e o objeto que **escapa**: se um ponteiro para memoria da arena sobrevive ao reset (guardado numa estrutura de vida longa, devolvido ao chamador), ele passa a apontar para memoria que sera sobrescrita pelo proximo uso — um use-after-free logico que nenhum `free` sinaliza. A regra de uso e: nada sai da fase sem ser copiado para memoria com dono proprio.

5. `malloc` tradicional continua melhor quando as vidas sao individuais e imprevisiveis, os tamanhos variam muito e o padrao nao se repete — por exemplo, um editor que mantem documentos abertos por tempo arbitrario, cada um com suas proprias estruturas. Arena e pool sao otimizacoes com pre-condicoes; fora delas, o alocador geral e a escolha honesta.

**Autoverificacao:** `usadas=24` apos as duas alocacoes, `hits=5 misses=1` no cenario montado e um exemplo seu de ponteiro escapando da arena descrito antes de rodar.
