---
layout: default
---

# Aula 27: pistas graduais

Volte aos [exercicios](../27.md#21-exercicios-obrigatorios) antes de abrir as solucoes.

1. `ptrvec_pop` devolve o ultimo ponteiro e diminui o tamanho; defina o que acontece com vetor vazio antes de codar.
2. O callback de comparacao recebe dois itens e devolve "iguais ou nao" para o seu criterio; a busca nao conhece o tipo apontado.
3. A fila sobre o vetor circular precisa de inicio e quantidade, como na [aula 13](13-solucoes.md).
4. Bytes por valor vencem quando o item e pequeno, de tamanho conhecido e copiado barato; `void *` vence quando o item e grande ou polimorfico.
5. No contrato de ownership: escreva quem cria, quem destroi e o que o container **nunca** faz com os itens.
