---
layout: default
---

# Aula 11: pistas graduais

Volte aos [exercicios](../11.md#16-exercicios-obrigatorios) antes de abrir as solucoes.

1. Compile com `-g -Og` para ter fonte e variaveis no depurador; no gdb, a sequencia minima e `break`, `run`, `next`, `print`.
2. Para o acesso fora dos limites: o programa "normal" pode nem falhar; o sintoma aparece no relatorio do sanitizer, nao na saida.
3. No valgrind, procure a linha `ERROR SUMMARY` e o primeiro contexto de leitura invalida.
4. Para o crash: deixe o programa cair dentro do gdb e leia a pilha de baixo para cima com `bt` ate achar o seu codigo.
5. Para a explicacao: separe "onde o programa estava quando parou" (depurador) de "qual acesso invalido aconteceu" (sanitizer).
