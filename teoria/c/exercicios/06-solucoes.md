---
layout: default
---

# Aula 06: solucoes comentadas

Compare apos tentar os [exercicios](../06.md#16-exercicios-obrigatorios).

1, 3 e 4. Programa em tres arquivos:

```c
// util.h
#ifndef UTIL_H
#define UTIL_H

#define MAXIMO(a, b) (((a) > (b)) ? (a) : (b))

int dobro(int x);

#endif
```

```c
// util.c
#include "util.h"

static int soma_um(int x)
{
    return x + 1;
}

int dobro(int x)
{
    return soma_um(x) + x;
}
```

```c
// main.c
#include <stdio.h>
#include "util.h"

int main(void)
{
    printf("dobro: %d\n", dobro(21));       /* 43 */
    printf("maximo: %d\n", MAXIMO(3 + 4, 5)); /* 7 */
    return 0;
}
```

Compilacao separada e ligacao:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -c util.c -o util.o
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -c main.c -o main.o
gcc util.o main.o -o programa
./programa
```

`soma_um` e `static`: visivel so dentro de `util.c`, nao entra no header e nao pode ser chamada de `main.c` — tentar chama-la de la produz erro de linkagem/declaracao, que e exatamente a fronteira de modulo pedida.

2. `MAXIMO(a, b)` parentiza cada parametro e a expressao completa, entao `MAXIMO(3 + 4, 5)` vira `(((3 + 4) > (5)) ? (3 + 4) : (5))`. O cuidado restante: os argumentos sao avaliados mais de uma vez quando o ramo os repete, entao `MAXIMO(i++, j)` tem efeito colateral repetido — macro nao e funcao; para argumentos com efeitos, prefira uma funcao.

5. `#include` e substituicao textual feita pelo pre-processador, antes de compilar: o compilador ve o conteudo do header copiado no ponto do `#include`. Isso explica a guarda (`#ifndef/#define/#endif`): sem ela, incluir o mesmo header duas vezes na mesma unidade de traducao duplicaria declaracoes. Tambem explica por que definicoes (nao declaracoes) em headers geram duplicacao na linkagem.

**Autoverificacao:** `dobro: 43` e `maximo: 7` na saida, um `.o` por fonte e `soma_um` inacessivel fora de `util.c`.
