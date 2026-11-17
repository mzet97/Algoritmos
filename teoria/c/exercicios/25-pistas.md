---
layout: default
---

# Aula 25: pistas graduais

Volte aos [exercicios](../25.md#20-exercicios-obrigatorios) antes de abrir as solucoes.

1. Arredondar por alinhamento e subir o tamanho pedido para o proximo multiplo de 8 antes de reservar; o desperdicio por alocacao fica abaixo de 8 bytes.
2. No pool, hit e servir de uma celula livre; miss e precisar alocar nova ou falhar — conte os dois na entrada da funcao, nao no retorno.
3. Arena contra pool no parser: os objetos de um request nascem juntos e morrem juntos, ou tem vidas individuais imprevisiveis?
4. Objeto que escapa da fase e o ponteiro que continua sendo usado depois do reset da arena.
5. `malloc` tradicional vence quando o padrao de vida nao cabe em "tudo morre junto" nem em "blocos do mesmo tamanho".
