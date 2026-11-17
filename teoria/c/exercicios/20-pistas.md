---
layout: default
---

# Aula 20: pistas graduais

Volte aos [exercicios](../20.md#17-exercicios-obrigatorios) antes de abrir as solucoes.

1. No `select`, o fd do servidor e os fds dos clientes vivem no mesmo conjunto; aceite so quando o servidor esta legivel.
2. No `poll`, cada cliente ganha uma entrada no array de `pollfd`; cliente que desconecta marca `fd = -1` ou sai do array.
3. Nao bloqueante muda o contrato: `accept`/`recv` podem devolver `-1` com `EAGAIN`, que significa "tente de novo depois", nao erro fatal.
4. Para a comparacao: quem recebe o resultado (conjunto de bits, array, fila de eventos) e o que acontece a cada chamada com muitos fds.
5. Para readiness: um socket legivel pode conter meia linha; prontidao nao e mensagem completa.
