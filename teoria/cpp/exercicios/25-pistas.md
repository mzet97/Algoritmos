---
layout: default
---

# Aula 25 (C++): pistas graduais

Volte aos [exercicios](../25.md#22-exercicios-obrigatorios) antes de abrir as solucoes.

1. Na versao template, o tipo do chamavel aparece na assinatura; na `std::function`, some atras de uma interface unica.
2. Type erasure e esconder o tipo concreto atras de operacoes fixas, guardando o objeto por ponteiro interno.
3. `std::function` pode alocar quando o chamavel e grande demais para o buffer interno (SBO varia por implementacao).
4. Ponteiro de funcao vence em C ABI, embarcados sem excecao e onde o custo precisa ser obvio.
5. `std::function` vence quando o tipo do chamavel so e conhecido na execucao (registro de plugins, tabela de callbacks).
