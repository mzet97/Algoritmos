---
layout: default
---

# Aula 28: pistas graduais

Volte aos [exercicios](../28.md#20-exercicios-obrigatorios) antes de abrir as solucoes.

1. Na soma com padding, cada elemento util ocupa uma linha quase inteira; compare bytes uteis por linha nos dois layouts.
2. Cache line e a unidade de transferencia, nao o tamanho do seu tipo; descubra o valor nesta maquina em vez de decorar.
3. SoA vence quando a varredura usa um ou dois campos de muitos registros; desenhe o que a linha de cache traz em cada layout.
4. False sharing nao precisa de variavel compartilhada: basta a mesma linha.
5. Sinais de layout ruim: ciclos altos com poucas instrucoes, `cache-misses` altos e speedup que nao vem quando voce adiciona nucleos.
