---
layout: default
---

# Aula 14 - Soluções comentadas e rubrica

## Contadores

[aula14.c](../exemplos/aula14.c) e a solucao executavel para mutex e atomic. A invariavel do contador protegido e: durante a secao critica, somente uma thread altera seu valor. O atomic evita data race no seu proprio objeto; memory_order_relaxed nao ordena publicacao de dados nao atomicos. Ler depois de pthread_join garante que as threads terminaram. Sem sincronizacao, ha comportamento indefinido; qualquer numero observado e inconclusivo.

## Fila com condvar

Uma solucao correta tem um mutex, um predicado fila_nao_vazia e um indicador encerrado. O produtor altera a fila e sinaliza a condvar sob um protocolo de mutex consistente. O consumidor faz:

~~~c
pthread_mutex_lock(&mutex);
while (fila_vazia && !encerrado)
    pthread_cond_wait(&cond, &mutex);
if (fila_vazia && encerrado) {
    pthread_mutex_unlock(&mutex);
    return;
}
item = retirar_da_fila();
pthread_mutex_unlock(&mutex);
~~~

Este trecho e conceitual: nomes e tipos da fila dependem da implementacao do aluno. A condvar nao armazena o evento; o predicado precisa ser reavaliado.

## Rubrica de 10 pontos

- 3: ausencia de data race e invariavel explicada.
- 2: criacao, join, erros e tempo de vida corretos.
- 2: condvar com predicado em loop e encerramento.
- 2: experimento reproduzivel e limites do TSan descritos.
- 1: comparacao entre mutex, atomic e condvar sem prometer ganho de desempenho.
