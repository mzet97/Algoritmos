---
layout: default
---

# Pistas da aula 30b

1. Kahan carrega uma compensação `c` que guarda o que cada soma perdeu no arredondamento e devolve na próxima. Compare o erro dela contra o oráculo em `double` — e cronometre: a compensação é uma cadeia de dependência única, como o somatório escalar.
2. Fixada, a thread não migra: cache quente e TLB estáveis derrubam a dispersão mesmo quando a média mal muda. Rode as 20 repetições intercaladas (uma fixada, uma livre) para o escalonador não favorecer um lado só da comparação.
3. Com `-ffast-math`, observe primeiro os erros contra o oráculo: se `erro_vs_double` muda sem você ter tocado no código, o flag reordenou suas somas. A frase de README honesta nomeia o que foi autorizado: reordenação associativa, ausência presumida de NaN/Inf e contração para FMA.
4. O desenho: aloque o vetor A no nó 0 (first-touch pela thread fixada no nó 0), o vetor B também, e depois meça o produto interno com a thread fixada no nó 1. Falta nesta máquina o segundo nó — o desenho fica no papel, rotulado como não executado.
