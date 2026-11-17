---
layout: default
---

# Aula 09: solucoes comentadas

Compare apos tentar os [exercicios](../09.md#15-exercicios-obrigatorios).

1, 2 e 4. Ponteiro de funcao, callback e `typedef` por papel:

```c
#include <stdio.h>

typedef double (*Operacao)(double);
typedef int (*Predicado)(double);
typedef void (*Acao)(double);

static double quadrado(double x)
{
    return x * x;
}

static double metade(double x)
{
    return x / 2.0;
}

static void aplica(double *v, size_t n, Operacao op)
{
    for (size_t i = 0; i < n; i++) {
        v[i] = op(v[i]);
    }
}

static size_t conta_se(const double *v, size_t n, Predicado p)
{
    size_t total = 0;
    for (size_t i = 0; i < n; i++) {
        if (p(v[i])) {
            total++;
        }
    }
    return total;
}

static int positivo(double x)
{
    return x > 0.0;
}

static void imprime(double x)
{
    printf("%.1f ", x);
}

static void para_cada(const double *v, size_t n, Acao a)
{
    for (size_t i = 0; i < n; i++) {
        a(v[i]);
    }
    putchar('\n');
}

int main(void)
{
    double v[] = {1.0, -2.0, 3.0};
    aplica(v, 3, quadrado);
    para_cada(v, 3, imprime);          /* 1.0 4.0 9.0 */
    printf("positivos: %zu\n", conta_se(v, 3, positivo));
    aplica(v, 3, metade);
    para_cada(v, 3, imprime);          /* 0.5 2.0 4.5 */
    return 0;
}
```

Os tres `typedef` nao sao intercambiaveis: `Operacao` transforma, `Predicado` decide e `Acao` produz efeito. Nomear o papel e o que simplifica a leitura sem esconder a diferenca.

3. Tabela de comandos nome → callback:

```c
#include <stdio.h>
#include <string.h>

typedef void (*Comando)(void);

static void cmd_ajuda(void)
{
    printf("comandos: ajuda, versao\n");
}

static void cmd_versao(void)
{
    printf("versao 1.0\n");
}

struct Entrada {
    const char *nome;
    Comando fn;
};

static const struct Entrada TABELA[] = {
    {"ajuda", cmd_ajuda},
    {"versao", cmd_versao},
};

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "uso: %s COMANDO\n", argv[0]);
        return 2;
    }
    for (size_t i = 0; i < sizeof TABELA / sizeof TABELA[0]; i++) {
        if (strcmp(argv[1], TABELA[i].nome) == 0) {
            TABELA[i].fn();
            return 0;
        }
    }
    fprintf(stderr, "comando desconhecido: %s\n", argv[1]);
    return 1;
}
```

Adicionar um comando e adicionar uma linha na tabela; o despachante nao muda.

5. O algoritmo (percorrer, contar, despachar) fica estavel enquanto a politica (qual operacao, qual predicado, qual comando) varia por chamada ou por linha de tabela. Sem callback, cada combinacao algoritmo × politica viraria uma funcao nova copiada — e cada correcao no algoritmo teria de ser repetida em todas.

**Autoverificacao:** saidas `1.0 4.0 9.0` e `0.5 2.0 4.5`, comando desconhecido com erro em `stderr` e nenhum aviso com `-Wall -Wextra`.
