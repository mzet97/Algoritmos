---
layout: default
---

# Aula 14c: soluções comentadas

Compare após tentar os [exercícios](../14c_cercas_e_seqlock.md#6-exercícios-obrigatórios). Experimentos sobre `teoria/rust/exemplos/14c_cercas.rs` com `rustc --edition 2024 -O -D warnings`.

1. Previsão: **passa** no x86-64 (verificado: 50 mil pares conferidos sem o `fence(Release)`). O TSO do x86 ordena stores — a barreira que falta ninguém sente *nesta* microarquitetura. "Passou aqui" prova o teste, não o programa: em ARM, sem barreira o par rasga. Portabilidade se argumenta pelo modelo de memória, não se testa numa máquina só — o `fence` existe para a máquina que você não tem.

2. Diagnóstico: **continua passando** 3/3 (verificado) — mas as retentativas despencam de ~1,1M para ~4k. Com a checagem de ímpar, toda leitura concorrente com escrita retenta (mesmo as que sairiam intactas); sem ela, só a efetivamente rasgada cai no `antes != depois`. Quem cobre o quê: `antes % 2` retenta concorrência, `antes != depois` pega rasgo. O buraco residual nas duas versões: ciclo completo par→par entre as duas leituras do contador — dado trocado com `antes == depois`. Com `Mutex` no dado, improvável ao ponto de nunca observado; sem ele, o seqlock clássico aceita a janela (ou alarga o contador para 64 bits e declara o risco).

3. Projeto: contadores separados (verificado): **~360k por ímpar, ~50 por mudança**. A checagem de ímpar é o grosso do "otimismo": retenta escrita em curso mesmo quando o dado sairia intacto — o leitor nunca trava o escritor, mas paga em retentativas. O `mudou` é o que realmente salva de rasgo (50 casos reais em 50 mil escritas). Otimismo tem preço mensurável — e agora o número está na saída, não na intuição.

4. Previsão: **passa igual** (verificado) — com `Release` no `store` e `Acquire` no `load`, o ordenamento vai junto com a operação e o `fence` vira redundante (tanto que o `use` precisa sair, ou o `-D warnings` barra). Cerca separada vale quando o dado não cabe numa operação atômica: aqui, o par `[u64; 2]` é não-atômico e o sinal é 1 bit — a cerca ordena sem carregar dado. Se o dado coubesse no próprio atômico, a cerca seria enfeite; em código concorrente, enfeite é dívida com cara de virtude.
