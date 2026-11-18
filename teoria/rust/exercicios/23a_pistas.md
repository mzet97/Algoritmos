---
layout: default
---

# Pistas da aula 23a

1. Fração paralelizável perto de 1 com N threads: teto `1/((1-p)+p/N)` antes da contenção. Calcule para p=0,99 e N=2,4,8 e compare com o medido; a diferença é banda + overhead + turbo.
2. Mude um `fetch_add` por elemento e meça de novo: cada adição disputa a mesma linha. Compare com a versão que soma local e publica uma vez.
3. Rode em `debug` e em `release` e anote a razão. O que muda não é o algoritmo — é o quanto o compilador participa.
4. Troque banda e kernel na fórmula e ache o par que vira o veredito. Discuta se ele existe no mundo real para *esta* carga.
