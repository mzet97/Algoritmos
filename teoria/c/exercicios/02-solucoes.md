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

**Autoverificacao:** programas compilam sem avisos com `-Wall -Wextra`, igualdade entre os dois numeros cai no ramo proprio e `-3` e classificado como impar.
