---
layout: default
---

# Aula 12: solucoes comentadas

Compare apos tentar os [exercicios](../12.md#16-exercicios-obrigatorios).

1 e 2. Biblioteca estatica com duas funcoes e um executavel que a usa:

```c
// arit.h
#ifndef ARIT_H
#define ARIT_H

int soma(int a, int b);
int produto(int a, int b);

#endif
```

```c
// arit.c
#include "arit.h"

int soma(int a, int b)
{
    return a + b;
}

int produto(int a, int b)
{
    return a * b;
}
```

```c
// main.c
#include <stdio.h>
#include "arit.h"

int main(void)
{
    printf("soma=%d produto=%d\n", soma(6, 7), produto(6, 7));
    return 0;
}
```

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -c arit.c -o arit.o
ar rcs libarit.a arit.o
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror main.c -L. -larit -o programa
./programa   # soma=13 produto=42
```

A ordem na ligacao importa: o `.a` vem **depois** do fonte/objeto que referencia os simbolos, porque o ligador resolve pendencias da esquerda para a direita.

3. `.o` e codigo objeto de uma unidade de traducao: instrucoes relocaveis e simbolos ainda por resolver. `.a` e um arquivo (indice + varios `.o`) do qual o ligador extrai so os membros necessarios. O executavel final ja passou pela ligacao: simbolos resolvidos, layout de memoria definido e, no caso dinamico, referencias nomeadas para as bibliotecas de execucao.

4. ABI (Application Binary Interface) e o contrato no nivel de maquina entre partes compiladas separadamente: tamanhos e alinhamento dos tipos, convencao de chamada (quais registradores e pilha carregam argumentos e retorno), formato do arquivo objeto e regras de nomes de simbolos. Codigo-fonte compativel pode ser binariamente incompativel quando a ABI muda.

5. Duas quebras classicas: mudar a assinatura de uma funcao publicada (um chamador antigo passa argumentos no layout antigo) e alterar ordem, tipo ou tamanho de campos de uma `struct` que atravessa a fronteira da biblioteca (os dois lados passam a ler deslocamentos diferentes). Em ambos os casos, recompilar so um lado nao basta — e por isso ABI estavel se trata como contrato versionado.

**Autoverificacao:** `soma=13 produto=42`, `ar t libarit.a` lista `arit.o` e remover o `-larit` produz erro de ligacao (simbolo nao definido), nao de compilacao.
