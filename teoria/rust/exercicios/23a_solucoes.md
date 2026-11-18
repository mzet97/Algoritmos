---
layout: default
---

# Soluções comentadas da aula 23a

## 1. Teto de Amdahl contra medido

Com p=0,99: N=2 → 1,98x; N=4 → 3,88x; N=8 → 7,48x. O medido fica abaixo: banda de memória (160 MB por passada divididos entre núcleos), criação de threads por `scope` a cada medição e turbo. Some `available_parallelism` à conta: com menos CPUs que workers, o teto cai antes de Amdahl.

## 2. `fetch_add` por elemento

Cada adição disputa a mesma linha de cache entre N núcleos: o programa continua correto e fica ordens de magnitude mais lento. A versão do exemplo soma local e publica uma vez por fatia — 4 operações atômicas no total. Moral: átomo não é o vilão; granularidade é.

## 3. `debug` contra `release`

Esperado: `debug` de 5x a 20x mais lento (sem vetorização, sem inlining agressivo, com checagens). Quem apresenta número de `debug` como resultado apresenta o custo do modo dev, não do programa. Benchmark sempre em `release` — e diga qual.

## 4. Conta que vira o veredito

`2 * bytes / banda + t_kernel < t_cpu`. Com 160 MB a 16 GiB/s: viagem ~19 ms; kernel de 1 ms não compensa CPU de ~10 ms. Para virar, precisa de banda muito maior *e* problema muito maior — ou kernel ordens de magnitude mais rápido. Mesma fórmula, conclusões opostas: a conta decide, não a torcida.
