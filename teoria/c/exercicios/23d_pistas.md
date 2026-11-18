---
layout: default
---

# Aula 23d: pistas graduais

Volte aos [exercícios](../23d_consenso_e_quorum.md#7-exercícios-obrigatórios) antes de abrir as soluções.

1. Com quorum 3, a fase 1 ainda passa (3 votos de 3). A fase 3 replica para 2 alcançáveis: 2 acks < 3. Qual `assert` cai? Pense em quem "todos" pune: não o particionado, o *sistema inteiro* — a maioria saudável perde a escrita confirmada por causa de um nó fora.
2. Com quorum 1, um voto basta. A minoria de 1 vota em si e... em quem o `assert` da fase 2 aposta? Se qualquer lado isolado confirma sozinho, desenhe duas partições confirmando valores opostos e diga qual sobrevive à cura — e o que isso prova sobre "qualquer um".
3. Quorum de 4 nós é 3. Cada lado da partição 2–2 soma no máximo 2 votos. Nenhum lado confirma: o sistema inteiro vira leitura. É esse o preço do par — e o motivo do ímpar (ou da testemunha): a indisponibilidade precisa cair de um lado só.
4. O termo do isolado avançou para 2 na derrota; o líder velho tem termo 1. Na cura sem eleição, quem tem termo maior? `replicar` pula quem não está no termo do líder — silenciosamente, sem erro. Replique 444 e compare `log`/`commit` de N2 com os demais: convergência de mentira.
