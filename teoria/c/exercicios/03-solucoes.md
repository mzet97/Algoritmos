---
layout: default
---

# Aula 03: solucoes comentadas

Compare apos tentar os [exercicios](../03.md#18-exercicios-obrigatorios).

1 e 2. Valor devolvido contra efeito colateral:

```c
#include <stdio.h>

int maior(int a, int b)
{
    return (a > b) ? a : b;
}

void linha(void)
{
    for (int i = 0; i < 20; i++) {
        putchar('-');
    }
    putchar('\n');
}

int main(void)
{
    printf("maior: %d\n", maior(7, 12));
    linha();
    return 0;
}
```

3. Troca com ponteiros: a funcao recebe os enderecos e escreve nos objetos do chamador:

```c
#include <stdio.h>

void troca(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int main(void)
{
    int x = 3, y = 9;
    troca(&x, &y);
    printf("x=%d y=%d\n", x, y); /* x=9 y=3 */
    return 0;
}
```

4. Potencia recursiva com pre-condicao explicita (`expoente >= 0`; `0^0` vale 1 por esta definicao):

```c
#include <stdio.h>

long potencia(int base, int expoente)
{
    if (expoente == 0) {
        return 1;
    }
    return (long)base * potencia(base, expoente - 1);
}

int main(void)
{
    printf("%ld\n", potencia(2, 10)); /* 1024 */
    return 0;
}
```

Resultados grandes estouram ate `long`; validar faixa e responsabilidade de quem chama neste exercicio.

5. `troca(int a, int b)` recebe **copias**: os parametros sao objetos novos inicializados com os valores do chamador. Trocar `a` e `b` dentro da funcao so rearranja as copias, que deixam de existir no retorno. Sem enderecos, a funcao nao tem como nomear os objetos originais — por isso a versao que funciona recebe `int *` e o chamador passa `&x, &y`.

**Autoverificacao:** `x=9 y=3` apos a troca, `potencia(2, 10) == 1024` e nenhum aviso com `-Wall -Wextra`.
