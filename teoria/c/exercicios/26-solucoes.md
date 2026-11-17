---
layout: default
---

# Aula 26: solucoes comentadas

Compare apos tentar os [exercicios](../26.md#21-exercicios-obrigatorios).

1, 2 e 3. Builder de bytes com `sb_append_char`, comprimento explicito e fator parametrizavel:

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct StrBuf {
    unsigned char *dados;
    size_t tamanho;
    size_t capacidade;
    double fator;
};

static int sb_reserva(struct StrBuf *sb, size_t extra)
{
    if (extra <= sb->capacidade - sb->tamanho) {
        return 0;
    }
    size_t nova = sb->capacidade == 0 ? 16 : sb->capacidade;
    while (nova - sb->tamanho < extra) {
        nova = (size_t)(nova * sb->fator) + 1;
    }
    unsigned char *novo = realloc(sb->dados, nova);
    if (novo == NULL) {
        return -1;
    }
    sb->dados = novo;
    sb->capacidade = nova;
    return 0;
}

static int sb_append_bytes(struct StrBuf *sb, const void *origem, size_t n)
{
    if (sb_reserva(sb, n) != 0) {
        return -1;
    }
    memcpy(sb->dados + sb->tamanho, origem, n); /* copia bytes, nao string */
    sb->tamanho += n;
    return 0;
}

static int sb_append_char(struct StrBuf *sb, unsigned char c)
{
    return sb_append_bytes(sb, &c, 1);
}

int main(void)
{
    const unsigned char binario[] = {0x41, 0x00, 0x42, 0x00, 0x43};
    for (int ensaio = 0; ensaio < 2; ensaio++) {
        struct StrBuf sb = {NULL, 0, 0, ensaio == 0 ? 2.0 : 1.5};
        for (int i = 0; i < 100; i++) {
            if (sb_append_char(&sb, 'x') != 0) {
                free(sb.dados);
                return 1;
            }
        }
        if (sb_append_bytes(&sb, binario, sizeof binario) != 0) {
            free(sb.dados);
            return 1;
        }
        printf("fator %.1f: tamanho=%zu capacidade=%zu zeros=%d meio=%c\n",
               sb.fator, sb.tamanho, sb.capacidade,
               sb.dados[101] == 0 && sb.dados[103] == 0,
               sb.dados[102]);
        free(sb.dados);
    }
    return 0;
}
```

Os bytes `0x00` no meio sao dados, nao terminadores: o tamanho final e 105 (100 + 5), nao a posicao do primeiro zero. E essa a diferenca entre um builder de **bytes** e um builder de strings — quem opera com `strlen` sobre esse buffer enxergaria "tamanho 1".

3. O fator entra como campo e a comparacao repete o desenho da [aula 24](24-solucoes.md): fator 2 realoca menos e pode parar com mais capacidade ociosa; 1.5 realoca mais e desperdiça menos no teto teorico.

4. `strlen` percorre a string ate o `\0` a cada chamada. Num loop que anexa e chama `strlen` para saber onde escrever, o custo total vira quadratico no tamanho final. O builder guarda `tamanho` como campo justamente para transformar essa pergunta em O(1) — comprimento e estado, nao algo a recomputar.

5. Guardar um ponteiro para dentro de `sb.dados` (por exemplo, "o inicio da ultima linha") e um risco classico: o proximo append pode realocar, `realloc` pode devolver outro endereco, e o ponteiro guardado passa a apontar para memoria liberada. Guarde **deslocamentos** (indices) e recompute o ponteiro apos cada crescimento — ou proiba crescimento enquanto houver ponteiros internos vivos, e documente.

**Autoverificacao:** tamanho 105 com zeros internos preservados, `sb_append_char` sem nenhum `strlen` e nenhum ponteiro interno guardado no seu desenho.
