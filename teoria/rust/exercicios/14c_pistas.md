---
layout: default
---

# Aula 14c: pistas graduais

Volte aos [exercícios](../14c_cercas_e_seqlock.md#6-exercícios-obrigatórios) antes de abrir as soluções.

1. Sem `fence(Release)`, as 50 mil publicações passam no x86-64 (verificado). O TSO garante ordem de stores — a cerca que falta ninguém sente *nesta* microarquitetura. "Passou" aqui prova o teste, não o programa: em ARM sem barreira, o par rasga. Portabilidade se argumenta, não se testa numa máquina só.
2. Sem a checagem de ímpar, continua passando 3/3 (verificado) — mas as retentativas despencam (~1,1M para ~4k). Com ímpar: toda leitura concorrente com escrita retenta. Sem: só a efetivamente rasgada (o `antes != depois` pega). O buraco residual nas duas: ciclo completo par→par entre as duas leituras do contador — dado trocado com `antes == depois`. Com `Mutex` no dado, improvável; sem ele, o seqlock clássico aceita a janela ou alarga o contador.
3. Contadores separados (verificado): ~360k por ímpar, ~50 por mudança. A checagem de ímpar é o grosso do "otimismo": retenta escrita em curso mesmo quando o dado sairia intacto. O `mudou` é o que realmente salva de rasgo. Otimismo tem preço mensurável — e agora você tem o número.
4. Com `Release`/`Acquire` no próprio sinal, passa igual (o ordenamento vai junto com a operação). Cerca separada vale quando o dado não cabe numa operação atômica (aqui: par `[u64; 2]` não-atômico + sinal de 1 bit). Se o dado coubesse no próprio atômico, a cerca seria enfeite — e enfeite em código concorrente é dívida que parece virtude.
