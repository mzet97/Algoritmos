---
layout: default
---

# Aula 39: pistas graduais

Volte aos [exercícios](../39.md#7-exercícios-obrigatórios) antes de abrir as soluções.

1. Com N=500 e incremento direto, sai `SMP_OK` 3/3 — e isso não prova nada. A janela leitura-modificação-escrita de 3 instruções raramente intercala em 500 voltas no QEMU. Teste que passa sem lock com N pequeno é sorte com(marker); N grande ou janela alargada é o que expõe. "Passou" sem adversário não é evidência.
2. Com a janela escancarada (200 iterações entre ler e escrever): `SOMA_DIVERGIU` 3/3 (verificado). Ler 500, somar 500, perder centenas no meio: cada hart sobrescreve o progresso do outro. E o lock original, com a mesma janela *dentro* da seção crítica, segue `SMP_OK` (verificado) — exclusão mútua não depende do tamanho da janela, só de quem pode estar dentro dela.
3. `lr.w.aq` reserva, `sc.w.rl` grava se a reserva sobreviveu (0 = conseguiu): `SMP_OK` (verificado). `amoswap` sempre escreve e gira no valor lido; `lr/sc` desiste antes de escrever quando vê ocupado — sob contenção alta, menos escrita inútil; sob contenção baixa, dá no mesmo. O `.aq`/`.rl` continua obrigatório nos dois: sem ordenação, o contador entra antes do lock na visão do outro hart.
4. Não trava mudo: sai `HART1_NAO_ANUNCIOU` + `0x3333` (verificado). O hart 1 gira no 2º `adquirir` segurando o lock; o hart 0 termina as suas 500, espera `hart1_ok` até o limite de 5M iterações e desiste com nome e código. Spinlock não é reentrante — adquirir o que já é seu é deadlock consigo mesmo. A espera limitada é o que converte esse deadlock em diagnóstico: sem limite, silêncio; com limite, causa nomeada.
