---
layout: default
---

# Aula 18: pistas graduais

Volte aos [exercicios](../18.md#16-exercicios-obrigatorios) antes de abrir as solucoes.

1. No header, declare `struct Contador;` sem definir; quem define e o `.c`, que tambem aloca.
2. `create` devolve o ponteiro ou `NULL`; `destroy` aceita `NULL` e libera tudo o que o objeto possui.
3. O callback recebe o evento e um `void *userdata` que a biblioteca nao interpreta — so devolve.
4. Para o `extern "C"`: pense em quem chama a funcao de C a partir de C++ e no que acontece com o nome do simbolo.
5. Para os riscos de FFI: comece por ownership de memoria, tipos com tamanhos diferentes entre linguagens e callbacks chamados com a convencao errada.
