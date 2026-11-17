---
layout: default
---

# Aula 29: solucoes comentadas

Compare apos tentar os [exercicios](../29.md#22-exercicios-obrigatorios). Programa de apoio (soma linear contra soma com stride sobre os mesmos dados):

```c
#include <stdio.h>
#include <stdlib.h>

static double sum_linear(const double *v, size_t n)
{
    double s = 0.0;
    for (size_t i = 0; i < n; i++) {
        s += v[i];
    }
    return s;
}

static double sum_stride(const double *v, size_t n, size_t passo)
{
    double s = 0.0;
    for (size_t i = 0; i < n; i += passo) {
        s += v[i];
    }
    return s;
}

int main(int argc, char *argv[])
{
    size_t n = argc > 1 ? (size_t)atol(argv[1]) : 8000000;
    double *v = malloc(n * sizeof *v);
    if (v == NULL) {
        return 1;
    }
    for (size_t i = 0; i < n; i++) {
        v[i] = 1.0;
    }
    printf("linear: %.0f\n", sum_linear(v, n));
    printf("stride8: %.0f\n", sum_stride(v, n, 8));
    free(v);
    return 0;
}
```

1 e 3. Roteiro executado:

```bash
gcc -std=c11 -O2 -g -o perfil perfil.c
perf stat -e cycles,instructions,cache-misses ./perfil
perf record ./perfil && perf report --stdio | head
```

O que conferir na saida da sua maquina (valores sao desta execucao e nao constantes): as duas somas conferem (`8000000` e `1000000`), `sum_stride` consome ciclos desproporcionais por elemento somado e `perf report` coloca o tempo onde as amostras estao — em cargas de um laco so, na propria funcao do laco.

2. Iteracao nao e a unidade de custo: com passo 8, cada iteracao toca uma linha diferente trazendo 64 bytes para usar 8; a soma linear toca 1 linha a cada 8 elementos. Menos passos com passos caros pode perder para mais passos baratos — o custo real mora em ciclos e faltas de cache, nao na contagem do fonte.

4. `perf record` amostra **onde** o tempo passa no seu processo; callgrind (valgrind) conta instrucoes por funcao e monta a arvore de chamadas: a distribuicao estrutural mostra quem chama quem e quanto cada aresta custa, ao preco de rodar muito mais lento e contar instrucoes em vez de ciclos reais. Um responde "onde dói na maquina", o outro "como o custo se organiza no programa".

5. `branch-misses` alto faria sentido se o laco tivesse um desvio dependente de dados imprevisiveis — por exemplo, somar condicionalmente valores aleatorios acima de um limiar. Nos lacos acima, os desvios sao os de controle do `for`, altamente previsiveis; um numero alto aqui indicaria outra coisa (ruido de medicao, carga vizinha).

6. `cache-misses` alto no stride: com passo de 8 doubles (64 bytes), cada acesso cai numa linha nova; a linha chega, entrega 8 bytes e o restante e descartado sem uso. O hardware prefetch tambem perde o padrao simples de "proxima linha", entao nem o mecanismo que salva a soma linear ajuda.

**Autoverificacao:** somas conferidas antes dos contadores, funcao quente lida do `perf report` e uma frase sua ligando passo, linha de 64 bytes e falta de cache.
