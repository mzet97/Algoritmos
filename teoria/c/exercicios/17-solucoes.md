---
layout: default
---

# Aula 17: solucoes comentadas

Compare apos tentar os [exercicios](../17.md#16-exercicios-obrigatorios).

1 e 2. Mesmo programa, tres niveis de otimizacao e perfil:

```c
#include <stdio.h>
#include <stdlib.h>

static double trabalho(int n)
{
    double s = 0.0;
    for (int i = 0; i < n; i++) {
        s += (i % 7) * 0.5;
    }
    return s;
}

int main(int argc, char *argv[])
{
    int n = (argc > 1) ? atoi(argv[1]) : 50000000;
    printf("soma=%.1f\n", trabalho(n));
    return 0;
}
```

```bash
for o in O0 O2 O3; do gcc -$o -std=c11 -o quente_$o quente.c; done
./quente_O0 200000000   # medido nesta maquina: ~0.50 s
./quente_O2 200000000   # medido nesta maquina: ~0.14 s
./quente_O3 200000000   # medido nesta maquina: ~0.14 s
perf stat -e task-clock ./quente_O2 200000000
```

As tres saidas imprimem o mesmo `soma=299999997.0` — resultado igual e pre-condicao do experimento. `-O2` e `-O3` empatam aqui porque o gargalo restante e o mesmo laco; `-O3` nao e "mais rapido por definicao", e um conjunto maior de transformacoes que so ajuda quando o gargalo combina com elas. Os tempos sao desta maquina e desta carga; o metodo (mesma entrada, varias repeticoes, resultado conferido) e a parte portatil. No `perf`, a funcao quente e `trabalho` — em programas reais, o perfil existe justamente porque a funcao quente costuma nao ser a que o autor imaginava. Protocolo completo de medicao: [aula 28a](../28a_benchmark_honesto.md).

3. Iterar um array percorre enderecos consecutivos: hardware prefetch e linhas de cache trabalham a favor, e o passo seguinte ja esta na maquina. Iterar uma lista ligada segue ponteiros para enderecos imprevisiveis: cada passo pode esperar a memoria principal. A contagem de operacoes e igual; o custo por operacao nao — ver [solucao da aula 13](13-solucoes.md).

4. Localidade espacial: quando o programa le um byte, a cache traz a linha inteira (tipicamente 64 bytes). Se os proximos acessos usam os vizinhos trazidos — elementos seguintes do array, campos proximos da struct — cada acesso caro paga varios acessos baratos. Sem esse reuso, a linha e buscada e descartada sem amortizar.

5. False sharing: dois nucleos escrevem variaveis **diferentes** que dividem a mesma linha de cache. O protocolo de coerencia invalida e transfere a linha a cada escrita, como se os nucleos disputassem o mesmo dado — aparece em contadores por thread alocados lado a lado num array, e some com preenchimento/alinamento que separa as linhas (medido na [aula 28a](../28a_benchmark_honesto.md)).

**Autoverificacao:** mesmo resultado nos tres niveis, funcao quente nomeada a partir do perfil (nao do palpite) e tempos declarados como desta maquina.
