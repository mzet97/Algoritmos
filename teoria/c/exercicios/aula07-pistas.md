---
layout: default
---

# Aula 07: pistas graduais

Volte aos [exercicios](../07.md#15-exercicios-obrigatorios) antes de consultar as solucoes.

1. Use `double *v = malloc(5 * sizeof *v);`; verifique `v == NULL` antes de acessar qualquer elemento.
2. `calloc(5, sizeof *v)` zera bytes. Compare resultados observaveis de elementos `double` apenas na sua plataforma; nao generalize a representacao de ponto flutuante.
3. Atribua o resultado de `realloc` a um ponteiro temporario. Em falha, o bloco antigo permanece alocado e precisa ser liberado pelo dono.
4. Antes de `n * sizeof *v`, teste `n > SIZE_MAX / sizeof *v`; escolha tambem um teto pratico menor. Teste `n == 0` e `n == SIZE_MAX` sem tentar de fato alocar esse ultimo valor.
5. Em ambos os casos ha erro de ciclo de vida: no vazamento falta liberar; no uso apos liberacao ha acesso a um objeto que ja perdeu a validade.
