---
layout: default
---

# Aula 18: solucoes comentadas

Compare apos tentar os [exercicios](../18.md#16-exercicios-obrigatorios).

1, 2 e 3. API com ponteiro opaco, ownership explicito e callback com `userdata`:

```c
// contador.h
#ifndef CONTADOR_H
#define CONTADOR_H

struct Contador; /* opaco: a definicao nao aparece aqui */

typedef void (*AvisoLimite)(void *userdata, long valor);

struct Contador *contador_create(long limite, AvisoLimite aviso, void *userdata);
void contador_destroy(struct Contador *c);
void contador_incrementa(struct Contador *c);
long contador_valor(const struct Contador *c);

#endif
```

```c
// contador.c
#include "contador.h"

#include <stdlib.h>

struct Contador {
    long valor;
    long limite;
    AvisoLimite aviso;
    void *userdata;
};

struct Contador *contador_create(long limite, AvisoLimite aviso, void *userdata)
{
    struct Contador *c = malloc(sizeof *c);
    if (c == NULL) {
        return NULL;
    }
    c->valor = 0;
    c->limite = limite;
    c->aviso = aviso;
    c->userdata = userdata;
    return c;
}

void contador_destroy(struct Contador *c)
{
    free(c); /* free(NULL) nao faz nada: destroy aceita NULL */
}

void contador_incrementa(struct Contador *c)
{
    c->valor++;
    if (c->valor == c->limite && c->aviso != NULL) {
        c->aviso(c->userdata, c->valor);
    }
}

long contador_valor(const struct Contador *c)
{
    return c->valor;
}
```

```c
// main.c
#include <stdio.h>
#include "contador.h"

static void meu_aviso(void *userdata, long valor)
{
    const char *nome = userdata;
    printf("%s atingiu %ld\n", nome, valor);
}

int main(void)
{
    struct Contador *c = contador_create(3, meu_aviso, (void *)"pedidos");
    if (c == NULL) {
        return 1;
    }
    contador_incrementa(c);
    contador_incrementa(c);
    contador_incrementa(c);
    printf("valor final: %ld\n", contador_valor(c));
    contador_destroy(c);
    contador_destroy(NULL);
    return 0;
}
```

Saida: `pedidos atingiu 3` e `valor final: 3`. O cliente nunca ve os campos: pode mudar a representacao sem recompilar quem usa o header (contrato de ABI da [aula 12](12-solucoes.md)). O `userdata` atravessa a biblioteca sem ser interpretado e volta no callback apontando para o contexto do cliente — e assim que C simula o que outras linguagens chamam de closure.

4. Em C++, o compilador codifica tipos dos parametros no nome do simbolo (name mangling) para permitir sobrecarga. Sem `extern "C"`, uma funcao declarada em C++ procura um simbolo decorado que a biblioteca C nao exporta, e a ligacao falha. `extern "C"` manda usar o nome simples de C na fronteira — util nas duas direcoes: chamar C de C++ e expor uma API C a partir de codigo C++.

5. Tres riscos de FFI mal desenhado: **ownership ambiguo** (ninguem sabe quem libera; vazamento ou liberacao dupla), **tipos que mudam de tamanho/representacao** entre as linguagens (`int`, strings sem terminador, structs com padding) e **callbacks com convencao de chamada ou vida-util erradas** (a biblioteca guarda um ponteiro para algo que o outro lado ja destruiu, ou chama de volta numa thread inesperada). Some-se: erros e panics nao podem atravessar a fronteira como excecoes — viram codigos de retorno ou comportamento indefinido.

**Autoverificacao:** o cliente compila sem conhecer os campos, `destroy(NULL)` e seguro e o callback recebe de volta exatamente o ponteiro registrado.
