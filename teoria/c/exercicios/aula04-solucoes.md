---
layout: default
---

# Aula 04: solucoes comentadas

Compare apos tentar os [exercicios](../04.md#19-exercicios-obrigatorios).

1. Um array de cinco elementos pode ser percorrido com `for (size_t i = 0; i < n; i++) printf("%d\n", v[i]);`, onde `n = sizeof v / sizeof v[0]` e calculado no escopo da declaracao `int v[5] = {1, 2, 3, 4, 5};`. Para `n == 0`, o corpo nao executa.
2. No mesmo laco, `*(v + i)` produz o mesmo elemento que `v[i]`. A equivalencia nao autoriza `i == n`.
3. `char texto[4] = {'A', 'n', 'a', '\0'};` e uma string C valida. O quarto byte e parte da capacidade, embora `strlen(texto)` devolva 3. Uma sequencia `char texto[3] = {'A', 'n', 'a'};` e apenas um buffer de tres bytes: chamar `strlen` nela pode ler fora do array.
4. `strlen` nao recebe capacidade; sua pre-condicao e encontrar `\0` em um objeto acessivel. Para bytes externos, primeiro localize o terminador dentro do comprimento conhecido ou use uma operacao limitada por esse comprimento.
5. `sizeof v` no mesmo escopo de `int v[5]` mede os bytes de cinco `int`; `sizeof parametro` em `void f(int parametro[])` mede o ponteiro recebido. Por isso a funcao recebe `n` separadamente. Compare os valores reais na sua plataforma sem presumir que um `int` tenha quatro bytes.

**Autoverificacao:** codigo sem avisos, indices dentro de `0..4`, string terminada dentro do objeto e comprimento explicito na chamada de funcao.
