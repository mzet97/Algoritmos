---
layout: default
---

# Aula 07: solucoes comentadas

Compare apos tentar os [exercicios](../07.md#15-exercicios-obrigatorios).

1. Uma solucao inicia `double *v = malloc(5 * sizeof *v);`, testa `v == NULL`, escreve `v[0]` ate `v[4]` e termina com `free(v)`. O conteudo inicial de `malloc` nao deve ser lido.
2. `double *v = calloc(5, sizeof *v);` pede cinco elementos e zera seus bytes. Em maquinas usuais com IEEE 754, ler esses `double` produz `0.0`, mas C nao garante esse resultado para toda representacao. Para portabilidade, atribua `0.0` a cada elemento antes de ler.
3. `double *tmp = realloc(v, 10 * sizeof *v); if (tmp == NULL) { free(v); return 1; } v = tmp;` preserva o bloco antigo no caminho de falha. Inicialize `v[5]` a `v[9]`; a extensao nao vem zerada. Ao final, chame `free(v)` uma vez.
4. Com `#include <stdint.h>`, cheque `n == 0 || n > SIZE_MAX / sizeof *v || n > LIMITE_PRATICO` antes de `malloc(n * sizeof *v)`. Para o experimento, `n = 0` e `n = SIZE_MAX` devem ser rejeitados antes da alocacao; `n = 5` deve seguir adiante e receber valores inicializados. Por exemplo, `LIMITE_PRATICO = 1000000` elementos, justificado pelo limite de memoria do exercicio. Uma entrada textual tambem requer conversao validada, assunto das aulas de I/O.
5. Vazamento perde a possibilidade de liberar o recurso, elevando uso de memoria ao longo do tempo. Uso apos liberacao acessa armazenamento cuja vida ja terminou e produz comportamento indefinido; e uma falha de seguranca potencial. Atribuir `NULL` a um alias nao corrige outros aliases.

**Autoverificacao:** explique o dono do bloco em cada linha de `realloc`, cubra os caminhos de falha e confirme que nenhuma multiplicacao de tamanho externo ocorre antes da verificacao.
