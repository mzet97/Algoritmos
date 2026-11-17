---
layout: default
---

# Aula 24: solucoes comentadas

Compare apos tentar os [exercicios](../24.md#21-exercicios-obrigatorios).

1. Crescimento 1.5 contra 2, medido no mesmo vetor:

```c
#include <stdio.h>
#include <stdlib.h>

struct Vetor {
    int *dados;
    size_t tamanho;
    size_t capacidade;
    size_t realocacoes;
    double fator;
};

static int empurra(struct Vetor *v, int x)
{
    if (v->tamanho == v->capacidade) {
        size_t nova = (v->capacidade == 0)
            ? 8
            : (size_t)(v->capacidade * v->fator) + 1;
        int *novo = realloc(v->dados, nova * sizeof *novo);
        if (novo == NULL) {
            return -1;
        }
        v->dados = novo;
        v->capacidade = nova;
        v->realocacoes++;
    }
    v->dados[v->tamanho++] = x;
    return 0;
}

int main(void)
{
    for (int ensaio = 0; ensaio < 2; ensaio++) {
        struct Vetor v = {NULL, 0, 0, 0, ensaio == 0 ? 2.0 : 1.5};
        for (int i = 0; i < 1000; i++) {
            if (empurra(&v, i) != 0) {
                free(v.dados);
                return 1;
            }
        }
        printf("fator %.1f: realocacoes=%zu capacidade=%zu ociosa=%zu\n",
               v.fator, v.realocacoes, v.capacidade,
               v.capacidade - v.tamanho);
        free(v.dados);
    }
    return 0;
}
```

Comportamento medido ao empurrar 1000 elementos nesta rodada: fator 2.0 realocou 8 vezes (capacidade final 1151, 151 ociosas); fator 1.5 realocou 13 vezes (capacidade 1232, 232 ociosas **neste ponto de parada**). A capacidade ociosa ao fim depende de onde o crescimento parou; o teto teorico dela e ~50% da capacidade com fator 2 e ~33% com fator 1.5. Nao ha vencedor universal — ha uma troca declarada entre copias de realocacao e memoria parada, e a escolha se faz com o perfil de crescimento da carga real.

2. Fragmentacao **interna**: pedir 13 bytes e receber um bloco de 16 — o desperdicio esta dentro do bloco, por arredondamento do alocador. Fragmentacao **externa**: liberar blocos alternados de 64 KB ate sobrar metade do heap livre em buracos de 64 KB; um pedido de 128 KB contiguos falha com memoria total suficiente. A primeira desperdiça por bloco; a segunda, por geometria.

3. Tres sinais de alocacao excessiva: `malloc`/`free` entre as funcoes mais quentes do perfil; RSS que sobe em degraus acompanhando picos de carga e nao desce; e copias de realocacao aparecendo como custo mensuravel em estruturas que crescem elemento a elemento sem reserva.

4. Objetos de vida longa alocados no meio de uma regiao de vida curta viram pinos: quando os vizinhos morrem, o buraco nao pode ser compactado nem devolvido (C nao move objetos), e o heap vira queijo suico em volta dos sobreviventes. Agrupar por tempo de vida — arenas por fase, como na [aula 25](25-solucoes.md) — ataca exatamente isso.

5. Capacidade extra controlada (reservar o tamanho esperado, crescer por fator) compra menos realocacoes e enderecos mais estaveis ao preco de memoria ociosa limitada e conhecida. E vantajosa quando o custo de copiar/mover supera o custo de segurar os bytes — e deixa de ser quando a capacidade reservada e chute sem relacao com a carga.

**Autoverificacao:** os dois fatores imprimem contagens diferentes de realocacao, a capacidade ociosa e calculada (nao estimada de olho) e cada definicao de fragmentacao vem com um exemplo numerico seu.
