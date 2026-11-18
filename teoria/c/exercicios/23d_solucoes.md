---
layout: default
---

# Aula 23d: soluções comentadas

Compare após tentar os [exercícios](../23d_consenso_e_quorum.md#7-exercícios-obrigatórios). Todos os experimentos partem de `teoria/c/exemplos/23d_consenso.c`, compilado com `gcc -std=c11 -Wall -Wextra -Wpedantic -Werror`.

1. Previsão: com `#define MAIORIA 3`, a fase 1 passa (3 votos), mas a fase 3 aborta em `replicar(nos, 0, maj, 2, 333) == 1` — 2 acks não alcançam 3. "Todos" pune o sistema saudável pela queda de um nó: a maioria de 2 perde a escrita confirmada. É por isso que quorum é maioria, não unanimidade.

2. Diagnóstico: com `#define MAIORIA 1`, a minoria de 1 vota em si, atinge 1 voto e elege líder — o `assert(lider_min == -1)` da fase 2 cai. "Qualquer um" confirma a escrita solitária 222 como se fosse verdade; com duas partições, cada lado confirma a sua e a cura precisa descartar uma confirmação — o que destrói o sentido de confirmar.

3. Projeto: com `#define NOS 4` e `#define MAIORIA 3`, inicialize 4 nós, eleja com os 4, particione `{0,1} | {2,3}` e tente eleger em cada lado: ambos somam 2 votos < 3, ambos falham, nada confirma. O sistema inteiro vira leitura. O quorum ímpar (3, 5) existe para que a indisponibilidade caia de um lado só: com 3 nós, a partição 2–1 mantém um lado confirmando.

4. Previsão: sem a eleição nova, a cura "passa" nos `assert`s — e mente. N2 voltou da partição com termo 2; o líder velho tem termo 1. Replicando 444 com todos após a cura:

```text
fase5 lider velho replica: ok=1 N2(t=2,log=2,commit=1) <- para tras
```

`ok=1` (N0 e N1 confirmaram 444) enquanto N2 ficou em `log=2,commit=1`: `replicar` pula em silêncio quem não está no termo do líder. O sistema imprime sucesso particionado. A eleição nova existe para unificar o termo antes de unificar os dados — sem ela, a convergência é de mentira.
