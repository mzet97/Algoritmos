---
layout: default
---

# Aula 30: solucoes comentadas

Compare apos tentar os [exercicios](../30.md#21-exercicios-obrigatorios).

1, 2 e 3. Os dois saxpy e o que o compilador relata sobre cada um:

```c
#include <stdio.h>
#include <stdlib.h>

static void saxpy_scalar(float a, const float *x, float *y, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        y[i] = a * x[i] + y[i];
    }
}

static void saxpy_branchy(float a, const float *x, float *y, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        if (x[i] > 0.0f) {
            y[i] = a * x[i] + y[i];
        }
    }
}

int main(void)
{
    const size_t n = 1000000;
    float *x = malloc(n * sizeof *x);
    float *y = malloc(n * sizeof *y);
    if (x == NULL || y == NULL) {
        free(x);
        free(y);
        return 1;
    }
    for (size_t i = 0; i < n; i++) {
        x[i] = (float)(i % 13) - 6.0f;
        y[i] = 1.0f;
    }
    saxpy_scalar(2.0f, x, y, n);
    saxpy_branchy(2.0f, x, y, n);
    printf("y[6]=%.1f y[7]=%.1f\n", (double)y[6], (double)y[7]);
    free(x);
    free(y);
    return 0;
}
```

```bash
gcc -std=c11 -O2 -fopt-info-vec -c saxpy.c -o /dev/null
gcc -std=c11 -O3 -march=native -fopt-info-vec -c saxpy.c -o /dev/null
```

`saxpy_scalar` e o candidato natural: corpo unico, sem desvio, iteracoes independentes — o relatorio de vetorizacao tipicamente o marca como vetorizado ja em `-O2`/`-O3`. `saxpy_branchy` condiciona cada elemento a um dado imprevisivel; vetorizar exige mascaras/blends e o compilador pode recusar ou vetorizar com custo que o perfil precisa julgar. O relatorio da **sua** maquina e a evidencia — anote versao do compilador e flags junto, como manda a [aula 28a](../28a_benchmark_honesto.md). E `-march=native` amarra o binario a esta CPU: distribuir esse artefato para maquinas mais antigas pode terminar em instrucao ilegal.

4. SoA ajuda mais que AoS quando o kernel le um campo so de muitos registros: atualizar `x` de um milhao de particulas em SoA varre um array contiguo de `float` (e alimenta o vetor SIMD sem espalhar); em AoS, cada elemento util chega misturado aos campos vizinhos e o registrador vetorial precisa de gathers/shuffles.

5. Sem `restrict`, o compilador deve presumir que `x` e `y` podem se sobrepor, e sobreposicao muda o significado de iteracoes futuras — isso forca versoes runtime de checagem ou impede vetorizar. Com `restrict`, voce promete que os ponteiros nao aliasam objetos modificados entre si, e o compilador agenda o laco sobre essa promessa; prometer errado e comportamento indefinido.

6. Intrinsicos se justificam quando ha medicao: o autovetorizador recusou ou gerou codigo aquem do necessario **neste** kernel quente, a plataforma-alvo e conhecida e o ganho medido paga a perda de portabilidade e legibilidade. Fora de caminho quente comprovado, e complexidade sem cliente — a ponte [30b](../30b_simd_e_afinidade.md) trata do modelo executado em seguida.

**Autoverificacao:** relatorio de vetorizacao colado com flags e versao, `y[6]` e `y[7]` coerentes com o sinal de `x` e uma frase sobre o preco de `-march=native`.
