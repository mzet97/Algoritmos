---
layout: default
---

# Aula 28: solucoes comentadas

Compare apos tentar os [exercicios](../28.md#20-exercicios-obrigatorios). Medicao completa e protocolo ficam na [aula 28a](../28a_benchmark_honesto.md); aqui, o experimento minimo:

1. Soma contigua contra soma com padding exagerado — mesmo resultado, geometria diferente:

```c
#include <stdio.h>
#include <stdlib.h>

struct ComPadding {
    double valor;
    char enchimento[56]; /* um elemento por linha de 64 bytes */
};

int main(void)
{
    const size_t n = 100000;
    double *contiguo = malloc(n * sizeof *contiguo);
    struct ComPadding *espalhado = malloc(n * sizeof *espalhado);
    if (contiguo == NULL || espalhado == NULL) {
        free(contiguo);
        free(espalhado);
        return 1;
    }
    for (size_t i = 0; i < n; i++) {
        contiguo[i] = 1.0;
        espalhado[i].valor = 1.0;
    }
    double soma_a = 0.0, soma_b = 0.0;
    for (size_t i = 0; i < n; i++) {
        soma_a += contiguo[i];
    }
    for (size_t i = 0; i < n; i++) {
        soma_b += espalhado[i].valor;
    }
    printf("sizeof ComPadding: %zu\n", sizeof(struct ComPadding));
    printf("somas: %.0f %.0f\n", soma_a, soma_b);
    free(contiguo);
    free(espalhado);
    return 0;
}
```

`sizeof` imprime 64: cada passo da segunda soma pode custar uma linha de cache inteira para 8 bytes uteis, enquanto a primeira traz 8 valores por linha. Cronometrar os dois lacos com o protocolo da 28a mostra o efeito nesta maquina; a razao de tempos nao e constante universal, a geometria e.

2. Cache line e a unidade minima que cache e memoria trocam entre si — tipicamente 64 bytes nas plataformas atuais de mesa e servidor. Nao e o tamanho do tipo nem o da pagina; e o "pacote" de cada falta de cache.

3. Struct of arrays vence array of structs quando a operacao varre **um campo** de muitos registros: somar `idade` de um milhao de pessoas em SoA le um array contiguo de inteiros; em AoS, le as structs inteiras, trazendo nome e endereco junto sem usa-los. Se a operacao usa o registro inteiro de cada vez, AoS volta a fazer sentido.

4. Race condition logica exige acesso concorrente ao mesmo dado com pelo menos uma escrita. False sharing dispensa isso: duas variaveis distintas na mesma linha, escritas por nucleos diferentes, geram o mesmo trafego de coerencia — o programa esta correto e lento, e nenhum detector de corrida aponta o problema, porque nao ha corrida; ha geometria.

5. Sinais de que layout/acesso estao derrotando o hardware: tempo que nao cai ao otimizar o algoritmo "no papel"; `cache-misses` altos no `perf stat` para uma carga que caberia em cache; e contadores vizinhos escritos por threads diferentes destruindo o escalonamento (item 4).

**Autoverificacao:** `sizeof` 64 conferido na sua maquina, somas iguais e uma explicacao que cite bytes uteis por linha, nao "lista e lenta".
