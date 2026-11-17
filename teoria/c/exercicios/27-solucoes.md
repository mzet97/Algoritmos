---
layout: default
---

# Aula 27: solucoes comentadas

Compare apos tentar os [exercicios](../27.md#21-exercicios-obrigatorios).

1, 2 e 3. Vetor de ponteiros com `pop`, busca com comparador e uma fila circular de ponteiros:

```c
#include <stdio.h>
#include <stdlib.h>

struct PtrVec {
    void **itens;
    size_t tamanho;
    size_t capacidade;
};

static int ptrvec_push(struct PtrVec *v, void *item)
{
    if (v->tamanho == v->capacidade) {
        size_t nova = v->capacidade == 0 ? 4 : v->capacidade * 2;
        void **novo = realloc(v->itens, nova * sizeof *novo);
        if (novo == NULL) {
            return -1;
        }
        v->itens = novo;
        v->capacidade = nova;
    }
    v->itens[v->tamanho++] = item;
    return 0;
}

static void *ptrvec_pop(struct PtrVec *v)
{
    if (v->tamanho == 0) {
        return NULL;
    }
    return v->itens[--v->tamanho]; /* nao libera o item: nao e dono */
}

typedef int (*Comparador)(const void *item, const void *alvo);

static long ptrvec_busca(const struct PtrVec *v, const void *alvo,
                         Comparador cmp)
{
    for (size_t i = 0; i < v->tamanho; i++) {
        if (cmp(v->itens[i], alvo)) {
            return (long)i;
        }
    }
    return -1;
}

static int igual_int(const void *item, const void *alvo)
{
    return *(const int *)item == *(const int *)alvo;
}

int main(void)
{
    int a = 10, b = 20, c = 30;
    struct PtrVec v = {NULL, 0, 0};
    ptrvec_push(&v, &a);
    ptrvec_push(&v, &b);
    ptrvec_push(&v, &c);
    printf("busca 20: %ld\n", ptrvec_busca(&v, &b, igual_int));
    printf("pop: %d\n", *(int *)ptrvec_pop(&v));
    printf("tamanho: %zu\n", v.tamanho);
    printf("pop vazio depois: %s\n",
           ptrvec_pop(&v) != NULL && ptrvec_pop(&v) != NULL &&
           ptrvec_pop(&v) == NULL ? "null" : "?");
    free(v.itens);
    return 0;
}
```

4. Guardar **bytes por valor** (um vetor de `struct` ou de arrays fixos) e melhor quando o item e pequeno e de tamanho conhecido: iteracao contigua, sem alocacao por item e sem perseguicao de ponteiro (aula 28). `void *` vence quando os itens sao grandes, heterogeneos ou ja pertencem a outra estrutura — copiar seria caro e duplicaria a verdade.

5. Contrato de ownership deste container, como ele deve ser documentado: o vetor e dono **apenas** do array de ponteiros (`free(v.itens)`); os itens apontados pertencem a quem os inseriu, que deve mante-los vivos enquanto estiverem no vetor e libera-los por conta propria. O container nunca da `free` nos itens — `ptrvec_pop` devolve o ponteiro sem liberar, e `free(v.itens)` nao toca nos itens. Quem violar (liberar um item ainda no vetor) cria um ponteiro pendurado que nenhuma funcao do container detecta.

A fila FIFO sobre vetor circular repete o desenho executado na [aula 13](13-solucoes.md) trocando `int` por `void *`: inicio + quantidade + modulo da capacidade.

**Autoverificacao:** `pop` em vetor vazio devolve `NULL` sem tocar memoria, a busca acha pelo criterio do callback (nao por identidade de ponteiro) e o contrato diz explicitamente o que o container nao libera.
