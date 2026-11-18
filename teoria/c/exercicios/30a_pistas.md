---
layout: default
---

# Pistas da aula 30a

1. O ingênuo lê `B` por colunas (passo de `n` doubles); o tiled lê faixas `BxB` que ficam residentes. Desenhe quais linhas de cache cada versão toca por iteração do laço interno.
2. Rode com `BLOCO` 8, 16, 32 e 64 e anote a razão. Bloco pequeno demais desperdiça reuso; grande demais estoura o cache. O ótimo depende da sua cache — meça, não chute.
3. Some `t_ida + t_kernel + t_volta` com a banda medida (não a do folheto). Se a viagem domina, nem kernel instantâneo salva: calcule o speedup mínimo exigido.
4. Troque a condição por outra dependente dos dados (ex.: `mexido[i] % 3 == 0`) e compare ordenado contra embaralhado. O padrão do dado manda mais que a forma do `if`.
