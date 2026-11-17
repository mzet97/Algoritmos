---
layout: default
---

# Aula 13 (C++): pistas graduais

Volte aos [exercicios](../13.md#16-exercicios-obrigatorios) antes de abrir as solucoes. (A ponte de seguranca e a [13a](13a_pistas.md).)

1. O acesso fora de faixa com `vector::operator[]` nao e checado; o ASan e quem vai nomear o endereco.
2. No leak, o valgrind separa "definitely lost" de memoria ainda alcancavel na saida.
3. No gdb: compile com `-g -Og`, `break main`, `run`, `next`, `print` na variavel do exemplo.
4. Crash e sintoma, UB e contrato violado, leak e recurso nao devolvido com programa "correto".
5. Dangling aponta para memoria devolvida que pode ser reutilizada a qualquer alocacao seguinte.
