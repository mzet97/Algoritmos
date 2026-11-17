---
layout: default
---

# Aula 14a — Concorrência em C: threads, memória e ciclo de vida

**Competências SDD:** CON, MEM e SEC. **Pré-requisitos:** aulas 07, 13a e 14. Você precisa saber `malloc`/`free`, invariantes e compilar com warnings. Não precisa saber rede, `mmap` avançado nem Assembly.

**Objetivo observável:** ao terminar, você deverá distinguir data race de corrida lógica no modelo de memória do C11, preservar uma invariante compartilhada com `pthread_mutex_t`, coordenar duas threads com variável de condição e explicar por que o argumento de uma thread precisa sobreviver à thread que o criou.

**Motivação:** a aula 14 apresentou pthreads; esta aula responde o que o padrão *garante*. Em C, ao contrário de Go ou Java, data race não é "comportamento indefinido pelo modelo com diagnóstico garantido" — é comportamento indefinido ponto final: o compilador pode assumir que isso nunca acontece e otimizar em cima. A prova de correção vem antes do teste, e o teste vem com sanitizers.

## 1. O modelo de memória em uma frase

Duas avaliações em threads diferentes, sem ordenação imposta entre elas, no mesmo objeto, com ao menos uma escrita: **data race, comportamento indefinido**. "Ordenação imposta" nasce de: criar/join de thread, `pthread_mutex_lock/unlock`, `pthread_cond_wait` com seu mutex, `pthread_once`, semáforos e operações atômicas de `<stdatomic.h>`. Todo o resto é torcida.

Distinção que salva projetos: data race é bug de *memória* (UB, sanitizer denuncia); corrida lógica é bug de *ordem* (resultado depende do escalonamento mesmo com sincronização perfeita). O sanitizer só enxerga o primeiro.

## 2. Cada thread tem sua stack; nenhuma tem seu endereço

Cada thread recebe uma pilha própria — mas todas vivem no **mesmo espaço de endereços**. Passar `&variavel_local` para uma thread e retornar da função antes do `join` é use-after-scope: a pilha foi embora, o endereço ficou. Regras:

- argumento da thread deve sobreviver até o `join` (estático, heap, ou escopo que aguarda);
- `pthread_join` sincroniza: tudo que a thread escreveu antes de retornar é visível após o `join`;
- thread destacada (`detach`) não pode ser colhida: use apenas quando ninguém precisa do resultado nem da prova de término.

## 3. Condição: sinalizar, não adivinhar

`pthread_cond_wait` libera o mutex e dorme atomicamente; ao acordar, **recheca o predicado em laço** — acordos espúrios existem, e `signal` sem predicado verdadeiro perde o sinal. O invariante mora no predicado (`pronto == 1`), não no `signal`.

## 4. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/14a_threads.c) contém exatamente o programa abaixo. Requer POSIX (`-pthread`):

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -pthread teoria/c/exemplos/14a_threads.c -o threads14a
./threads14a
```

```c
#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N_THREADS 8
#define N_REP 100000L

/* Contador com invariante: todo acesso sob mutex. */
typedef struct {
    pthread_mutex_t mu;
    long total;
} Contador;

static void *somar(void *arg) {
    Contador *c = arg;
    for (long i = 0; i < N_REP; i++) {
        pthread_mutex_lock(&c->mu);
        c->total++;
        pthread_mutex_unlock(&c->mu);
    }
    return NULL;
}

/* Handoff ordenado: produtor avisa, consumidor espera o predicado. */
typedef struct {
    pthread_mutex_t mu;
    pthread_cond_t cond;
    int pronto;
    int valor;
} Canal;

static void *produtor(void *arg) {
    Canal *c = arg;
    pthread_mutex_lock(&c->mu);
    c->valor = 42;
    c->pronto = 1;
    pthread_cond_signal(&c->cond);
    pthread_mutex_unlock(&c->mu);
    return NULL;
}

static void *consumidor(void *arg) {
    Canal *c = arg;
    pthread_mutex_lock(&c->mu);
    while (!c->pronto) {
        pthread_cond_wait(&c->cond, &c->mu);
    }
    int v = c->valor;
    pthread_mutex_unlock(&c->mu);
    /* Devolve pelo heap: sobrevive ao termino da thread. */
    int *saida = malloc(sizeof *saida);
    assert(saida != NULL);
    *saida = v;
    return saida;
}

int main(void) {
    /* 1. Contador: 8 x 100000, invariante exata. */
    Contador c;
    assert(pthread_mutex_init(&c.mu, NULL) == 0);
    c.total = 0;
    pthread_t ts[N_THREADS];
    for (int t = 0; t < N_THREADS; t++) {
        assert(pthread_create(&ts[t], NULL, somar, &c) == 0);
    }
    for (int t = 0; t < N_THREADS; t++) {
        assert(pthread_join(ts[t], NULL) == 0);
    }
    /* O &c acima e seguro: main aguarda todos os joins antes de sair. */
    assert(c.total == (long)N_THREADS * N_REP);
    printf("contador: %ld (esperado %ld)\n", c.total, (long)N_THREADS * N_REP);
    assert(pthread_mutex_destroy(&c.mu) == 0);

    /* 2. Handoff produtor -> consumidor com predicado em laco. */
    Canal canal;
    assert(pthread_mutex_init(&canal.mu, NULL) == 0);
    assert(pthread_cond_init(&canal.cond, NULL) == 0);
    canal.pronto = 0;
    canal.valor = 0;
    pthread_t prod, cons;
    assert(pthread_create(&cons, NULL, consumidor, &canal) == 0);
    assert(pthread_create(&prod, NULL, produtor, &canal) == 0);
    void *ret = NULL;
    assert(pthread_join(cons, &ret) == 0);
    assert(pthread_join(prod, NULL) == 0);
    assert(ret != NULL && *(int *)ret == 42);
    printf("handoff: valor = %d\n", *(int *)ret);
    free(ret);
    assert(pthread_mutex_destroy(&canal.mu) == 0);
    assert(pthread_cond_destroy(&canal.cond) == 0);

    printf("threads ok: invariantes preservadas\n");
    return EXIT_SUCCESS;
}
```

Saída esperada (tempos variam; os números, não):

```text
contador: 800000 (esperado 800000)
handoff: valor = 42
threads ok: invariantes preservadas
```

Leia com o modelo certo: o `assert` do contador prova a invariante para *esta* execução; a ausência de data race para *todas* vem do argumento (todo acesso sob `mutex`), não dos testes. O `&c` passado às threads é seguro porque `main` sobrevive aos `join`s — o mesmo endereço passado a uma thread destacada seria use-after-scope.

### O que a memória faz aqui?

Oito pilhas de thread, um heap compartilhado e um espaço de endereços único. O mutex serializa o acesso ao contador; a condição coordena sem busy-wait. O `malloc` do resultado do consumidor existe porque a pilha da thread morre com ela: devolver endereço de variável local da thread seria o bug clássico que o `assert(ret != NULL && ...)` não pegaria sozinho — o sanitizer de endereços, sim, se o tempo de vida fosse violado.

## 5. Erros comuns e implicações defensivas

1. **Argv da thread na stack que retorna:** use-after-scope silencioso. Heap, estático ou `join` antes do retorno.
2. **Esquecer `join`/`detach`:** thread destacada sem dono e thread unida sem colheita vazam recurso do SO.
3. **`pthread_cond_wait` sem laço:** acordo espúrio lê `pronto == 0` como se fosse 1. Predicado sempre em `while`.
4. **`signal` sem predicado:** sinal antes da espera se perde. O estado mora na variável, não no sinal.
5. **Achar teste verde prova ausência de race:** em C, data race é UB — teste que passa hoje otimiza amanhã. Prova primeiro, sanitizer depois.
6. **Ignorar retorno de `pthread_*`:** falha de criação/lock silenciosa vira invariante violada longe dali. Cheque com `assert` no didático, com erro tratado no real.

## 6. Ponte com Python, Go e C++

Python teria o GIL no caminho do contador (aula 16a) e `threading.Lock` no lugar do mutex; Go detectaria corrida com `-race` (aula 11a); C++ oferece as mesmas primitivas com `std::thread` (aula 17a). O modelo muda de roupa; a disciplina é igual: dono dos dados, fechamento/coleta explícita, predicado em laço.

## 7. Exercícios obrigatórios

1. Reescreva as fontes de happens-before em pthreads sem consultar a seção 1: criação/join, mutex, condvar, once.
2. Remova o mutex do contador e preveja o resultado. Rode com `-fsanitize=thread` onde ele funcionar e transcreva o WARNING.
3. Troque o `while` do predicado por `if` e explique um cenário concreto em que o consumidor lê lixo.
4. Faça o produtor sinalizar *antes* de o consumidor esperar e mostre por que nada se perde (dica: onde mora o estado?).

Use [pistas graduais](exercicios/14a_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/14a_solucoes.md) depois de registrar sua própria explicação.

## 8. Critério de aprovação e checklist

Você concluiu a aula se consegue, sem olhar a solução:

- definir data race no modelo C11 e distingui-lo de corrida lógica;
- explicar por que o argumento da thread precisa sobreviver ao `join`;
- enunciar o papel do predicado em laço na condvar;
- rodar o exemplo com ASan/UBSan limpo e explicar cada invariante;
- produzir um WARNING do TSan sob demanda onde ele funcionar;
- apontar o que o teste prova e o que só o argumento prova.

**Próxima aula:** sockets e programação de sistemas Unix-like. O raciocínio desta aula sustenta servidores: cada conexão precisa de dono, ciclo de vida e invariante.
