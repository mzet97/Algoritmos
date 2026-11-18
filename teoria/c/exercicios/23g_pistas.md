---
layout: default
---

# Aula 23g: pistas graduais

Volte aos [exercícios](../23g_antientropia.md#9-exercícios-obrigatórios) antes de abrir as soluções.

1. As chaves 3, 30 e 63 caem em faixas distintas nos dois grãos (0, 1/3, 3/7). O contador mostra os mesmos 3 e 3 — porque ele conta faixas divergentes e entradas *movidas*. O que ele não conta: as 15 irmãs intactas que viajaram em cada faixa de 16. Grão maior não muda o mostrado; muda o desperdiçado.
2. Sem `versao <` na comparação, o cenário 2 (a→b) copia as 8 entradas da faixa sobre `b` — inclusive a chave 40, que volta de 9999/v2 para 400/v1. O `assert` de "0 entradas" cai. Resumo diz *onde*; versão diz *para onde*. Sem a segunda, a sincronização apaga o mais novo.
3. Troque o tipo do hash e as duas constantes; o resto (comparação, cópia, versão) não muda. Os três cenários passam byte-idênticos — porque nenhum deles testa colisão, só protocolo. O que você comprou: aniversário de ~77 mil para ~5 bilhões de faixas. O que você não comprou: garantia — 64 bits também colidem, só não na sua vida.
4. Após o cenário 3 as lojas são `iguais`: 8 resumos de 32 bits decidem tudo, 0 faixas viajam. Esse é o custo do estado convergido — e o motivo pelo qual anti-entropia pode rodar a cada minuto: no caso comum ela custa comparações, não cópias.
