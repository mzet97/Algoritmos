---
layout: default
---

# Aula 17: pistas graduais

Volte aos [exercicios](../17.md#16-exercicios-obrigatorios) antes de abrir as solucoes.

1. Use o mesmo fonte e a mesma entrada nos tres niveis; se o resultado mudar, o experimento esta errado, nao o compilador.
2. No `perf`, procure a funcao que concentra os ciclos; em programa de um laco so, ela deve ser obvia — confirme em vez de presumir.
3. Para array contra lista: conte bytes contiguos por passo e saltos de memoria por passo.
4. Localidade espacial: uma linha de cache traz vizinhos que voce ainda vai usar; quem paga e quem se beneficia?
5. False sharing mora em variaveis diferentes na mesma linha de cache escritas por nucleos diferentes; desenhe dois contadores vizinhos.
