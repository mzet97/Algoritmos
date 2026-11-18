---
layout: default
---

# Aula 32d: pistas graduais

Volte aos [exercícios](../32d_vfs_minimo.md#9-exercícios-obrigatórios) antes de abrir as soluções.

1. `/dado` tem 4 caracteres; `dados` tem 5: `m == n` falha antes do conteúdo. Barra dupla some no `while (*p == '/') p++` — o andador nem percebe. Uma regra nega por comprimento, a outra tolera por pulo: prefixo nunca casa, separador repetido nunca atrapalha.
2. Sem a guarda, `..` vira componente comum: procurado entre os filhos da raiz, não achado, `-1` do mesmo jeito. O teste passa igual — e é essa igualdade que engana. A guarda nega antes de existir busca; sem ela, a negação depende de a árvore *não conter* um filho chamado `..`. Monte outro suporte com esse nome e a busca acharia.
3. Uma linha na tabela + três syscalls na tarefa: nenhum toque no `resolver`, no `fd_alocar`, nas regras. Se o quarto arquivo resolve, lê e fecha, o andador provou ser genérico — a árvore cresceu e o código de resolução nem ficou sabendo.
4. Fechar marca `fd_no[id] = -1`; o segundo `close` e o `read` batem no guarda `fd_no[id] < 0` e voltam `-1`. Se aceitassem, o `read` copiaria do arquivo errado (ou do lixo) e o `close` duplo liberaria o slot de outro dono. Guarda de fd é invariante de tabela, não cortesia.
