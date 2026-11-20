---
layout: default
---

# Aula 13a — Busca, ordenação, hash e o custo da abstração

Buscar e ordenar parecem problemas resolvidos até a entrada crescer mil vezes. Esta ponte mede o custo real de cada estratégia e ensina a pergunta certa: não 'qual é o mais rápido?', mas 'rápido para qual entrada, a que custo de memória?'

Você vai ao terminar, você deverá implementar busca binária com invariante declarado, uma tabela hash de endereçamento aberto que recusa crescer além da capacidade contratada, e um teste de propriedade determinístico que confronta sua ordenação com `qsort`. Tudo com `size_t` verificado, sem overflow silencioso na conta de alocação.

a aula 13 mostrou pilha, fila, lista e tabela simples. Falta a pergunta que separa estrutura que funciona de estrutura que se pode defender: *qual é o invariante que cada operação preserva, quanto custa quando a entrada cresce, e o que acontece quando a conta de memória estoura?* O `malloc(nmemb * size)` da aula 07 já avisou: multiplicar tamanhos sem verificar é convidar o overflow. Esta aula fecha o módulo C3.

## 1. Contrato antes do código

Busca binária sobre vetor ordenado de `int`:

- **Pré-condição:** `v[0..n)` ordenado em ordem não decrescente; `n` cabe em `size_t`.
- **Pós-condição:** se o valor existe, o índice devolvido aponta para ele; se não existe, o resultado é `-1` como `long`.
- **Caso especial:** `n == 0` devolve `-1` sem tocar em `v` (ponteiro pode ser `NULL`).

Tabela hash desta aula (didática, capacidade fixa de 64 posições):

- **Pré-condição:** chaves são `unsigned int`; a tabela foi criada por `tabela_criar`.
- **Pós-condição:** `inserir` torna `buscar` capaz de achar a chave, a menos que a tabela esteja cheia — caso em que `inserir` devolve fracasso explícito em vez de corromper.
- **Caso especial:** capacidade esgotada não realoca sozinha; quem chama decide (esta aula documenta o limite em vez de escondê-lo).

## 2. Invariantes que sobrevivem ao laço e à sonda

Na busca binária, `ini` e `fim` prometem: se o valor está no vetor, está em `v[ini..fim)`. Cada descarte preserva a promessa pela ordenação; `fim - ini` diminui estritamente; ao esvaziar, `-1` é correto.

Na tabela de sondagem linear, o invariante é sobre a *sequência de sondagem*: `buscar(k)` percorre exatamente as mesmas posições que `inserir(k)` percorreria, na mesma ordem, parando no primeiro slot vazio ou na chave. Remoção ingênua (esvaziar o slot) quebraria a sequência de quem colidiu depois — por isso esta implementação didática **não remove**: documenta a limitação em vez de fingir que remoção com sondagem linear é trivial. Remoção correta (tombstones ou rehash da cauda) fica como exercício com teste.

## 3. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/13a_estruturas.c) contém exatamente o programa abaixo:

```c
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAB_CAP 64u

/* LCG deterministico: sem dependencia externa, semente fixa. */
static uint32_t lcg_estado = 42u;

static uint32_t lcg_proximo(void) {
    lcg_estado = lcg_estado * 1664525u + 1013904223u;
    return lcg_estado >> 8;
}

/* Busca binaria: pre-condicao vetor ordenado em ordem nao decrescente. */
static long busca_binaria(const int *v, size_t n, int alvo) {
    size_t ini = 0;
    size_t fim = n;
    while (ini < fim) {
        /* ini + (fim - ini) / 2 nunca soma dois grandes: sem overflow. */
        size_t meio = ini + (fim - ini) / 2;
        if (v[meio] == alvo) {
            return (long)meio;
        }
        if (alvo < v[meio]) {
            fim = meio;
        } else {
            ini = meio + 1;
        }
    }
    return -1L;
}

/* Tabela hash didatica: sondagem linear, capacidade fixa, sem remocao. */
typedef struct {
    unsigned int chave;
    int valor;
    int ocupado; /* 0 = livre, 1 = ocupado */
} Slot;

typedef struct {
    Slot slots[TAB_CAP];
    size_t usadas;
} Tabela;

static void tabela_iniciar(Tabela *t) {
    memset(t, 0, sizeof *t);
}

/* Devolve 1 e grava, ou 0 se a tabela estiver cheia (fracasso explicito). */
static int tabela_inserir(Tabela *t, unsigned int chave, int valor) {
    if (t->usadas >= TAB_CAP) {
        return 0;
    }
    size_t pos = (size_t)chave % TAB_CAP;
    for (size_t i = 0; i < TAB_CAP; i++) {
        size_t s = (pos + i) % TAB_CAP;
        if (!t->slots[s].ocupado || t->slots[s].chave == chave) {
            if (!t->slots[s].ocupado) {
                t->usadas++;
            }
            t->slots[s].chave = chave;
            t->slots[s].valor = valor;
            t->slots[s].ocupado = 1;
            return 1;
        }
    }
    return 0;
}

static int tabela_buscar(const Tabela *t, unsigned int chave, int *saida) {
    size_t pos = (size_t)chave % TAB_CAP;
    for (size_t i = 0; i < TAB_CAP; i++) {
        size_t s = (pos + i) % TAB_CAP;
        if (!t->slots[s].ocupado) {
            return 0;
        }
        if (t->slots[s].chave == chave) {
            *saida = t->slots[s].valor;
            return 1;
        }
    }
    return 0;
}

static int comparar_int(const void *a, const void *b) {
    int x = *(const int *)a;
    int y = *(const int *)b;
    return (x > y) - (x < y);
}

int main(void) {
    assert(busca_binaria(NULL, 0, 5) == -1L);
    int um[] = {5};
    assert(busca_binaria(um, 1, 5) == 0L);
    assert(busca_binaria(um, 1, 4) == -1L);

    /* Propriedade: ordena copia com qsort e confere busca em tudo. */
    for (int t = 0; t < 500; t++) {
        size_t n = (size_t)(lcg_proximo() % 51u);
        /* Guarda contra overflow da conta de alocacao: n * sizeof(int). */
        if (n > SIZE_MAX / sizeof(int)) {
            fputs("tamanho grande demais\n", stderr);
            return EXIT_FAILURE;
        }
        int *v = malloc(n * sizeof *v);
        if (n > 0 && v == NULL) {
            fputs("falha ao alocar\n", stderr);
            return EXIT_FAILURE;
        }
        for (size_t i = 0; i < n; i++) {
            v[i] = (int)(lcg_proximo() % 201u) - 100;
        }
        qsort(v, n, sizeof *v, comparar_int);
        for (size_t k = 0; k < n; k++) {
            assert(busca_binaria(v, n, v[k]) >= 0L);
        }
        assert(busca_binaria(v, n, 1000) == -1L);
        assert(busca_binaria(v, n, -1000) == -1L);
        free(v);
    }

    /* Hash: insere 0..63 (enche), a 64a recusa; tudo inserido e achavel. */
    Tabela tab;
    tabela_iniciar(&tab);
    for (unsigned int k = 0; k < TAB_CAP; k++) {
        assert(tabela_inserir(&tab, k, (int)(k * 10u)));
    }
    assert(tabela_inserir(&tab, TAB_CAP, 1) == 0);
    for (unsigned int k = 0; k < TAB_CAP; k++) {
        int achado = 0;
        assert(tabela_buscar(&tab, k, &achado) && achado == (int)(k * 10u));
    }
    int lixo = 0;
    assert(!tabela_buscar(&tab, TAB_CAP + 1000u, &lixo));

    /* Medicao: linear x binaria sobre um milhao de elementos ordenados. */
    const size_t grande_n = 1000000u;
    int *grande = malloc(grande_n * sizeof *grande);
    assert(grande != NULL);
    for (size_t i = 0; i < grande_n; i++) {
        grande[i] = (int)(i * 2u);
    }
    clock_t t0 = clock();
    volatile long r_lin = -2L;
    for (int r = 0; r < 5; r++) {
        for (size_t i = 0; i < grande_n; i++) {
            if (grande[i] == 1999999) {
                r_lin = (long)i;
                break;
            }
        }
        r_lin = -1L; /* ausente: percorre tudo, o pior caso */
    }
    double t_lin = (double)(clock() - t0) / CLOCKS_PER_SEC / 5.0;
    t0 = clock();
    volatile long r_bin = -2L;
    for (int r = 0; r < 200; r++) {
        r_bin = busca_binaria(grande, grande_n, 1999999);
    }
    double t_bin = (double)(clock() - t0) / CLOCKS_PER_SEC / 200.0;
    assert(r_lin == -1L && r_bin == -1L);
    printf("linear: %.6fs/media (5x)\n", t_lin);
    printf("binaria: %.9fs/media (200x)\n", t_bin);
    free(grande);
    return EXIT_SUCCESS;
}
```

Compile e execute:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/13a_estruturas.c -o estruturas
./estruturas
```

Saída esperada (tempos variam por máquina; a razão importa, não o dígito):

```text
linear: ~0.00Xs/media (5x)
binaria: ~0.0000000Xs/media (200x)
```

O oráculo aqui é `qsort` da biblioteca padrão: ordenamos a cópia com ele e conferimos que a busca acha tudo. Note o teste de capacidade da hash: encher 64 posições e ver a 65ª recusada com `0` prova o fracasso explícito em vez de corrupção silenciosa.

### O que a memória faz aqui?

Cada vetor de teste é alocado com `malloc(n * sizeof *v)` após verificar `n > SIZE_MAX / sizeof(int)` — a conta de alocação é o ponto clássico de overflow (C-03). O `free` fecha cada ciclo; nada vaza por design, e o sanitizer de endereços confirma. A tabela vive na stack do `main` (`Tabela tab`), 64 slots fixos: sem `malloc`, sem `free`, sem fragmentação — e sem crescimento, o que o contrato declara.

## 4. Erros comuns e implicações defensivas

1. **Somar limites sem pensar:** `ini + fim` pode estourar; `ini + (fim - ini) / 2` nunca soma dois grandes.
2. **Remover da hash com sondagem sem rehash:** esvaziar o slot quebra a sequência de quem colidiu depois. Sem tombstones, remoção é corrupção adiada.
3. **Contar alocação sem verificar:** `n * sizeof *v` estoura antes do `malloc` ver. Verifique antes de multiplicar.
4. **Comparador de `qsort` com subtração:** `return x - y` estoura para extremos. Use `(x > y) - (x < y)`.
5. **Medir antes de provar:** benchmark de busca errada mede a velocidade do erro.
6. **Semente solta:** LCG com estado global fixo é reproduzível; `rand()` sem `srand` fixo ou `time(NULL)` como semente gera falha irreproduzível.

## 6. Exercícios obrigatórios

1. Reescreva a prova do invariante da busca binária (início, preservação, término) sem consultar a seção 2.
2. Implemente remoção com tombstones na tabela e prove que `buscar` continua percorrendo a sequência correta. Meça o custo de uma tabela cheia de tombstones.
3. Troque a semente do LCG para `7` e `1234`; o resultado muda? Explique o que `srand(time(NULL))` quebraria.
4. Escreva uma versão da busca que devolve o *primeiro* índice em caso de repetidos e prove o invariante ajustado.

Use [pistas graduais](exercicios/13a_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/13a_solucoes.md) depois de registrar sua própria explicação.

## 7. Ao final, você consegue

Você concluiu a aula se consegue, sem olhar a solução:

- declarar pré-condição, pós-condição e convenção de repetidos, de vetor vazio e de tabela cheia;
- explicar por que o invariante vale antes e depois de cada iteração e de cada sonda;
- indicar a grandeza que diminui e provar o término;
- compilar e executar com warnings como erro e confrontar `O(log n)` com a medição;
- explicar por que o teste de propriedade usa `qsort` como oráculo e LCG com semente fixa;
- apontar onde a conta de alocação pode estourar e como o código se protege.


**Referências primárias:** *Introduction to Algorithms* (Cormen et al.), capítulos de busca, hash e ordenação; [N1570, seção 7.22.3](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf) para a conta de alocação.

**Próxima aula:** concorrência com pthreads. O raciocínio por invariantes desta aula sustenta invariantes compartilhadas: o que cada thread pode assumir sobre a estrutura.

## 5. Ponte com Python e JavaScript

Python entrega `bisect`, `sorted` e `dict` prontos; JavaScript entrega `Array.prototype.sort` (cuidado: sem comparador ordena como texto) e `Map`. A identidade do invariante é a mesma nas três linguagens. O que muda: em C, cada garantia custa código explícito — ordenação, capacidade, overflow, liberação. Nas outras, a estrutura pronta cobra o preço escondido (realocações, hash, GC). Saber implementar em C ensina o que o preço esconde.

---

### Para se aprofundar

1. Brian Kernighan e Dennis Ritchie, *C Programming Language* (Prentice Hall, 2. ed., ISBN 978-0131103627); 2. *C11 standard draft N1570* ([open-std.org](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf)); 3. Randall Bryant e David O'Hallaron, *Computer Systems: A Programmer's Perspective* (Pearson, 3. ed., ISBN 978-0134092669).
