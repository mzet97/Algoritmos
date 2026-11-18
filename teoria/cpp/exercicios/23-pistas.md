---
layout: default
---

# Aula 23 (C++): pistas graduais

Volte aos [exercicios](../23.md#20-exercicios-obrigatorios) antes de abrir as solucoes. (O endurecimento executado e a ponte [23a](23a_pistas.md).)

1. `ECHO texto` e um ramo do despachante: o protocolo continua sendo linha terminada em `\n`.
2. O limite do `input_buffer` se confere a cada append, antes de interpretar; excedeu, fecha.
3. A fila de saida guarda mensagens inteiras; o envio parcial consome da primeira e so entao avanca.
4. Progresso parcial: `send`/`recv` parciais sao o caso normal, nao a excecao.
5. Estado por conexao real: buffers, deslocamentos, prazo de ociosidade, estado de autenticacao e do protocolo.
