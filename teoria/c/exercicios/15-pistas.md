---
layout: default
---

# Aula 15: pistas graduais

Volte aos [exercicios](../15.md#17-exercicios-obrigatorios) antes de abrir as solucoes.

1. O loop de aceite envolve `accept`: cada retorno e um cliente novo, atendido e fechado antes do proximo.
2. Eco e devolver os mesmos bytes recebidos com `send` ate completar o total; `recv` pode devolver menos do que voce pediu.
3. Trocar a porta muda so o valor dado a `htons`; documente por que a ordem de bytes da rede nao e a da sua maquina.
4. Protocolo por linha: acumule bytes ate encontrar `\n` antes de interpretar; defina um teto para a linha.
5. Para a explicacao: monte o exemplo de duas mensagens que chegam num `recv` so, e de uma mensagem partida em dois.
