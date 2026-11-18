---
layout: default
---

# Aula 23g: soluções comentadas

Compare após tentar os [exercícios](../23g_antientropia.md#9-exercícios-obrigatórios). Experimentos sobre `teoria/c/exemplos/23g_antientropia.c` com `gcc -std=c11 -Wall -Wextra -Wpedantic -Werror`.

1. Previsão: com `FAIXAS = 4`, o cenário 3 mostra as mesmas **3 faixas e 3 entradas** (verificado) — as chaves 3, 30 e 63 continuam em faixas distintas (0, 1, 3). O contador esconde o desperdício: cada faixa divergente agora carrega **16 entradas** em vez de 8 — 15 intactas viajando de carona por faixa (verificado via impressão do grão). Contador igual, custo por faixa dobrado. É assim que se dimensiona o grão: pela divergência típica observada, nunca chutada.

2. Diagnóstico: sem a guarda `b->e[i].versao < a->e[i].versao`, o cenário 2 (a→b) copia as 8 entradas da faixa sobre `b` — a chave 40 volta de `9999/v2` para `400/v1` e o `assert(faixas == 1 && entradas == 0)` cai (verificado). A escrita mais nova morreu por sincronização. Resumo decide o **escopo** (qual faixa); versão decide a **direção** (para onde). Um sem o outro é metade do protocolo — e a metade que falta apaga dado.

3. Projeto: com FNV-1a 64 bits, os três cenários passam com saída byte-idêntica (verificado). Mudou: resumo de 4→8 bytes por faixa (64 bytes por rodada em vez de 32) e aniversário de ~77 mil para ~5 bilhões de faixas. Não mudou: a possibilidade de colisão — nem o fato de que nenhum dos três cenários a exercita. Upgrade de digest compra raridade, não garantia; a honestidade editorial (contrato "quase sempre" + re-check listado) continua valendo.

4. Previsão: **0 faixas, 0 entradas** (verificado). Após o cenário 3 as lojas são `iguais`, e 8 comparações de 64 bits decidem tudo sem mover um byte. Esse zero é o argumento econômico da aula: sincronização que custa comparações no caso comum pode rodar a cada minuto; a que custa cópias só roda "quando dá" — e "quando dá" é como divergências grandes nascem.
