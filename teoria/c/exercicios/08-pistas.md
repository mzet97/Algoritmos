---
layout: default
---

# Aula 08: pistas graduais

Volte aos [exercicios](../08.md#16-exercicios-obrigatorios) antes de abrir as solucoes.

1. Conte linhas contando `\n` lidos com `fgets` em buffer fixo; decida o que fazer com uma ultima linha sem `\n`.
2. Na copia com `fgets`/`fputs`, o laco continua enquanto `fgets` nao devolve `NULL`; confira escrita e fechamento dos dois arquivos.
3. Na copia binaria, `fread` devolve itens lidos, nao bytes pedidos; escreva exatamente o que foi lido a cada rodada.
4. Erros vao para `stderr` com `fprintf(stderr, ...)` e, quando houver `errno`, com o texto de `perror`.
5. Para a explicacao: relacione buffering com o custo de uma syscall por caractere.
