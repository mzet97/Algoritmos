---
layout: default
---

# Aula 20 (C++): pistas graduais

Volte aos [exercicios](../20.md#20-exercicios-obrigatorios) antes de abrir as solucoes.

1. Reactor **reage** a prontidao e chama seu handler; proactor entrega a operacao **concluida**.
2. Handler de timer separado registra prazo e callback; o loop acorda no menor prazo.
3. Thread por conexao pode ser simples com poucas conexoes longas e bloqueantes; fica inadequado quando milhares estao ociosas.
4. Estado por conexao: buffers de entrada/saida, quanto ja foi enviado, prazo e estado do protocolo.
5. Callback captura o mundo por referencia; se o dono morrer antes, a continuacao acorda sobre memoria morta.
