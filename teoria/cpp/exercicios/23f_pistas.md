---
layout: default
---

# Aula 23f: pistas graduais

Volte aos [exercícios](../23f_eleicao_commit_rpc.md#7-exercícios-obrigatórios) antes de abrir as soluções.

1. `push_back` direto cresce o `log` sem tocar no `commit`: `size()=1`, `commit=-1`. Confirmar é ato do quorum (`acks >= QUORUM` marca `commit`), não efeito de existir no vetor. A invariante: `commit` só anda por maioria — o resto é rascunho com termo.
2. Com quorum 1, um voto basta: a minoria de 1 elege, e `replicar` com 1 ack confirma a solitária. Duas partições confirmariam duas verdades — e a cura teria de descartar uma *confirmação*, não um rascunho. É a fronteira entre "perder dado" e "quebrar promessa".
3. `std::map` + vetor de ordem: ao exceder 2, apaga `ordem.front()`. `req-3` expulsa `req-1`; reenviar `req-1` reexecuta (`execucoes` vai a 4, `repeticao=false`). Idempotência com cache limitado vale até o despejo — depois, retry vira reexecução. Em produção, TTL ou confirmação do cliente; aqui, o teste prova o preço.
4. `!nos[id].vivo` pula na contagem de acks: o commit avança com os vivos, e N2 fica com `log` menor até a cura copiar o log do líder. Morto não confirma nem é confirmado — mas continua membro: ao voltar, segue (recebe o log). Quorum conta vivos, não cadeiras.
