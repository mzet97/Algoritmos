---
layout: default
---

# Aula 02: solucoes comentadas

Compare apos tentar os [exercicios](../02.md#18-exercicios-obrigatorios).

1. Maior de dois inteiros, com igualdade tratada:

```c
#include <stdio.h>

int main(void)
{
    int a, b;
    if (scanf("%d %d", &a, &b) != 2) {
        fprintf(stderr, "entrada invalida\n");
        return 1;
    }
    if (a > b) {
        printf("maior: %d\n", a);
    } else if (b > a) {
        printf("maior: %d\n", b);
    } else {
        printf("iguais: %d\n", a);
    }
    return 0;
}
```

2. Par ou impar. O resto de numero negativo pode ser negativo (`-3 % 2 == -1`), por isso a comparacao correta e com zero, nao com `1`:

```c
#include <stdio.h>

int main(void)
{
    int n;
    if (scanf("%d", &n) != 1) {
        return 1;
    }
    printf("%s\n", (n % 2 == 0) ? "par" : "impar");
    return 0;
}
```

3. Menu com `switch`; sem `break`, a execucao cai no proximo `case`:

```c
#include <stdio.h>

int main(void)
{
    int opcao;
    if (scanf("%d", &opcao) != 1) {
        return 1;
    }
    switch (opcao) {
    case 1:
        printf("novo\n");
        break;
    case 2:
        printf("abrir\n");
        break;
    case 3:
        printf("sair\n");
        break;
    default:
        printf("opcao invalida\n");
        break;
    }
    return 0;
}
```

4. Divisao inteira trunca em direcao a zero; basta um operando `double` para a divisao ser em ponto flutuante:

```c
#include <stdio.h>

int main(void)
{
    printf("inteira: %d\n", 7 / 2);     /* 3 */
    printf("double:  %f\n", 7.0 / 2);   /* 3.500000 */
    printf("cast:    %f\n", (double)7 / 2);
    return 0;
}
```

Converter depois nao recupera a fracao: `(double)(7 / 2)` vale `3.0`, porque a divisao inteira ja aconteceu.

5. Em C, condicoes testam escalares: `0` e falso e qualquer valor diferente de zero e verdadeiro. E assim para inteiros, ponto flutuante (`0.0` e falso) e ponteiros (`NULL` e falso). Por isso `if (n)` e `if (n != 0)` sao equivalentes para `int n` — e por isso comparacoes explicitas costumam deixar a intencao mais clara.

6. Diagnosticos: (a) imprime `Verdadeiro` e `x` passa a valer 5 — atribuicao, nao comparacao; o aviso e `suggest parentheses around assignment used as truth value [-Wparentheses]`; (b) imprime `0`: `a` e convertido para `unsigned int` antes da comparacao (`-1` vira 4294967295) pela regra das conversoes aritmeticas usuais; o aviso sem o pragma e `comparison of integer expressions of different signedness [-Wsign-compare]`; (c) imprime `0`: `0.1 + 0.2` vale `0.30000000000000004` em binario, e `==` apos aritmetica de ponto flutuante compara o erro junto — igualdade de flutuante se testa por faixa, nunca por bit.

7. Calculadora (esqueleto com os pontos cobrados):

```c
#include <stdio.h>

int main(void)
{
    int opcao;
    double a, b;
    for (;;) {
        printf("1 somar 2 subtrair 3 multiplicar 4 dividir 0 sair\n");
        if (scanf("%d", &opcao) != 1)
            return 1;
        if (opcao == 0)
            break;
        if (scanf("%lf %lf", &a, &b) != 2)
            return 1;
        switch (opcao) {
        case 1:
            printf("%.2f\n", a + b);
            break;
        case 2:
            printf("%.2f\n", a - b);
            break;
        case 3:
            printf("%.2f\n", a * b);
            break;
        case 4:
            if (b != 0.0 && (a / b == a / b))
                printf("%.2f\n", a / b);
            else
                printf("divisao invalida\n");
            break;
        default:
            printf("opcao invalida\n");
            break;
        }
    }
    return 0;
}
```

A guarda `b != 0.0 &&` usa curto-circuito: com `b == 0`, o segundo operando nem e avaliado. Note que `a / b == a / b` e falso quando o resultado e NaN (`0.0 / 0.0`) — a comparacao com ela mesma detecta o indefinido sem tocar em `errno`.

8. Laboratorio: experimento 1 entrega arquitetura + compilador + tabela `sizeof`/limites com a conclusao padrao-vs-plataforma; experimento 2 entrega previsao escrita antes de cada run, aviso transcrito onde houver (`-Wformat`, `-Wsign-compare`, `-Wparentheses`) e a regra que cada um ensina; experimento 3 entrega os dois programas com a mesma saida, a invariante (`npares` conta pares de `[0,i)`) e a condicao de parada (`i` alcanca `N` sem pular o incremento) escritas.

**Autoverificacao:** programas compilam sem avisos com `-Wall -Wextra`, igualdade entre os dois numeros cai no ramo proprio e `-3` e classificado como impar.
