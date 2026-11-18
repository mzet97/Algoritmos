---
layout: default
---

# Aula 03 (Rust): pistas graduais

Volte aos [exercicios](../03.md#14-exercícios-obrigatórios) antes de abrir as solucoes.

1. `&String` empresta; a prova e usar a string depois da chamada.
2. Para inverter por caracteres (nao bytes): colete `chars`, inverta e reconstrua.
3. Na cadeia: quem e dono depois de `criar`, depois de `processar` e em `finalizar`?
4. Nos trechos: escalares sao `Copy`; `String` move; `&str` e uma referencia copiavel para dados que vivem em outro lugar.
5. Primeira palavra em **bytes**: o indice de um espaco num `&str` e indice de byte, nao de caractere.
6. Trocar por valores copiados nao toca os originais — a mesma conversa da [aula 03 de C](../../c/exercicios/03-solucoes.md).
