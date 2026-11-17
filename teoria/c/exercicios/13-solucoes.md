---
layout: default
---

# Aula 13: solucoes comentadas

Compare apos tentar os [exercicios](../13.md#16-exercicios-obrigatorios).

1, 2 e 3. Pilha com `peek`, fila circular e remocao no inicio da lista, em um programa so:

```c
#include <stdio.h>
#include <stdlib.h>

#define CAP 4

struct Pilha {
    int dados[CAP];
    size_t topo; /* quantidade de elementos */
};

static int pilha_push(struct Pilha *p, int x)
{
    if (p->topo == CAP) {
        return -1;
    }
    p->dados[p->topo++] = x;
    return 0;
}

static int pilha_peek(const struct Pilha *p, int *saida)
{
    if (p->topo == 0) {
        return -1;
    }
    *saida = p->dados[p->topo - 1];
    return 0;
}

struct Fila {
    int dados[CAP];
    size_t inicio;
    size_t quantidade;
};

static int fila_push(struct Fila *f, int x)
{
    if (f->quantidade == CAP) {
        return -1;
    }
    f->dados[(f->inicio + f->quantidade) % CAP] = x;
    f->quantidade++;
    return 0;
}

static int fila_pop(struct Fila *f, int *saida)
{
    if (f->quantidade == 0) {
        return -1;
    }
    *saida = f->dados[f->inicio];
    f->inicio = (f->inicio + 1) % CAP;
    f->quantidade--;
    return 0;
}

struct No {
    int valor;
    struct No *proximo;
};

static int lista_remove_inicio(struct No **cabeca, int *saida)
{
    if (*cabeca == NULL) {
        return -1;
    }
    struct No *antigo = *cabeca;
    *saida = antigo->valor;
    *cabeca = antigo->proximo;
    free(antigo);
    return 0;
}

int main(void)
{
    struct Pilha p = {{0}, 0};
    int x;
    pilha_push(&p, 7);
    pilha_push(&p, 8);
    if (pilha_peek(&p, &x) == 0) {
        printf("peek: %d\n", x); /* 8, sem remover */
    }
    if (pilha_peek(&p, &x) == 0) {
        printf("peek de novo: %d\n", x); /* 8 */
    }

    struct Fila f = {{0}, 0, 0};
    fila_push(&f, 1);
    fila_push(&f, 2);
    fila_push(&f, 3);
    fila_pop(&f, &x);
    fila_push(&f, 4);
    fila_push(&f, 5); /* reutiliza o inicio do array */
    while (fila_pop(&f, &x) == 0) {
        printf("fila: %d\n", x); /* 2 3 4 5 */
    }

    struct No *lista = NULL;
    for (int i = 3; i >= 1; i--) {
        struct No *novo = malloc(sizeof *novo);
        if (novo == NULL) {
            return 1;
        }
        novo->valor = i;
        novo->proximo = lista;
        lista = novo;
    }
    while (lista_remove_inicio(&lista, &x) == 0) {
        printf("lista: %d\n", x); /* 1 2 3 */
    }
    return 0;
}
```

`peek` difere de `pop` so por nao decrementar `topo` — chamar duas vezes devolve o mesmo valor. Na fila, o contador `quantidade` distingue cheia de vazia quando `inicio` volta ao mesmo indice. Na lista, remover o inicio e O(1) porque so a cabeca muda; o no removido e liberado exatamente uma vez.

4. Para iteracao longa, o array dinamico guarda elementos contiguos: cada linha de cache traz varios proximos elementos, e o custo por passo tende a ser o da leitura sequencial. A lista ligada guarda um no por alocacao, espalhado no heap: cada passo e uma perseguicao de ponteiro que pode falhar o cache e ainda carrega o ponteiro `proximo` junto. A lista vence em insercao/remocao com iterador em maos, nao em varredura.

5. Localidade importa porque a memoria principal e ordens de grandeza mais lenta que o cache, e o cache move linhas (tipicamente 64 bytes), nao elementos. Estrutura que concentra os dados acessados juntos transforma um acesso caro em varios baratos; estrutura que espalha paga o acesso caro por elemento. E esse efeito — nao a contagem de operacoes — que costuma decidir iteracoes grandes, como a [aula 13b](../13b_arvores_e_ordenacao.md) mede.

**Autoverificacao:** `peek` repetido sem remover, fila imprimindo `2 3 4 5` apos reutilizar o anel e lista esvaziada sem vazamento no valgrind/ASan.
