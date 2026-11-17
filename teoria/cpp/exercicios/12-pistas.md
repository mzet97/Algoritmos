---
layout: default
---

# Aula 12 (C++): pistas graduais

Volte aos [exercicios](../12.md#16-exercicios-obrigatorios) antes de abrir as solucoes.

1. Header guarda declaracoes e a guarda de inclusao; a definicao da funcao comum mora em **um** `.cpp`.
2. Template mora no header porque o compilador precisa do corpo em cada unidade que o usa.
3. Para o erro de link: apague a definicao, recompile e leia qual simbolo ficou indefinido — compilacao passa, ligacao nao.
4. ODR: uma definicao por programa para funcoes/variaveis nao-inline; declaracoes, quantas quiser.
5. ABI repete a conversa da [aula 12 de C](../../c/exercicios/12-solucoes.md), com name mangling somado.
