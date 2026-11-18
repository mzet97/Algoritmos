---
layout: default
---

# Aula 39: soluções comentadas

Compare após tentar os [exercícios](../39.md#7-exercícios-obrigatórios). Variantes sobre `teoria/cpp/kernel39/`, compiladas com as flags do `Makefile` do estágio e rodadas no QEMU com `-smp 2`.

1. Previsão: com incremento direto e N=500, sai **`SMP_OK` 3/3** (verificado) — e isso é sorte, não prova. A sequência leitura-modificação-escrita tem 3 instruções; em 500 voltas, o QEMU raramente intercala os dois harts dentro dela. Teste que passa sem adversário não é evidência: N pequeno com janela estreita mede a preguiça do escalonador do hospedeiro, não a correção do programa. Quem conclui "lock desnecessário" com N=500 conclui sem experimento.

2. Diagnóstico: com a janela escancarada (ler, girar 200, escrever): **`SOMA_DIVERGIU` 3/3** (verificado). Cada hart lê 500, soma 500 na cópia local e sobrescreve o progresso do outro — centenas de incrementos evaporam. E o lock original, com a mesma janela *dentro* da seção crítica: **`SMP_OK`** (verificado). Exclusão mútua não depende do tamanho da janela — depende de só um hart estar dentro dela por vez. Janela alargada é o microscópio; o lock é o espécime que sobrevive a ele.

3. Projeto: com `lr.w.aq`/`sc.w.rl` e retry: **`SMP_OK`** (verificado). O laço: reserva (`lr`), desiste se ocupado (`tmp != 0`), tenta gravar (`sc`), repete se a reserva morreu (`tmp != 0`). Diferença real: `amoswap` sempre escreve (troca o valor e gira no lido); `lr/sc` só escreve se ninguém tocou — sob contenção alta, menos tráfego de escrita inútil; sob contenção baixa, equivalente. O `.aq`/`.rl` continua obrigatório: sem acquire na aquisição e release na liberação, o contador entra na visão do outro hart antes do lock — e o experimento volta ao exercício 1 com outro nome.

4. Previsão: **diagnóstico nomeado, não trava muda** (verificado: `HART1_NAO_ANUNCIOU` + `0x3333`). O hart 1 gira no 2º `adquirir` segurando o lock na mão; o hart 0 termina as 500, espera `hart1_ok` até o limite de 5M iterações e desiste com nome e código. Spinlock não é reentrante: adquirir o que já é seu é deadlock consigo mesmo, e segurar o lock enquanto gira é deadlock com o outro. A espera limitada é o que converte deadlock em diagnóstico — sem limite, dois harts girando em silêncio para sempre; com limite, causa nomeada e código de saída.
