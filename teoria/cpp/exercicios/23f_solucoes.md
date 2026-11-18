---
layout: default
---

# Aula 23f: soluções comentadas

Compare após tentar os [exercícios](../23f_eleicao_commit_rpc.md#7-exercícios-obrigatórios). Experimentos sobre `teoria/cpp/exemplos/23f_commit.cpp` com `g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror`.

1. Previsão: `log.size() = 1`, `commit = -1` (verificado). O `push_back` direto escreve o rascunho sem passar pelo quorum — e o `commit` nem fica sabendo. A invariante que o `commit` protege: só a maioria transforma rascunho em promessa. Presença no `log` do líder é intenção; confirmação é o carimbo de `acks >= QUORUM`. Quem lê `log` sem olhar `commit` lê intenção como promessa.

2. Diagnóstico: com `QUORUM = 1`, a minoria de 1 **elege** — o `assert(eleger(...) == -1)` da fase 2 cai (verificado). Um voto basta, e `replicar` com 1 ack confirmaria a solitária. Duas partições confirmariam duas verdades opostas — e a cura teria de descartar uma *confirmação*, destruindo o sentido de confirmar. "Qualquer um" não perde dado: quebra promessa.

3. Projeto: com teto de 2 e despejo do mais antigo, `req-3` expulsa `req-1`; reenviar `req-1` reexecuta (`execucoes = 4`, `repeticao = false` — verificado, fases originais intactas). O preço do teto: idempotência com memória limitada vale até o despejo — retry após despejo vira reexecução silenciosa. Em produção, o cliente confirma recebimento (e o servidor só despeja o confirmado) ou o `id` carrega TTL; aqui o teste prova o preço em vez de escondê-lo. Detalhe de implementação que o exercício revela: `std::map` ordena por chave, não por idade — a ordem de inserção precisa do vetor auxiliar.

4. Previsão: o morto é **pulado** (`continue` em `!vivo`), o commit **avança** sem ele, e N2 fica para trás (`log` menor até a cura copiar o log do líder — verificado). Morto não vota nem confirma — mas continua membro: ao voltar (`vivo = true`), segue o líder na cura. Quorum conta vivos, não cadeiras: a aritmética da maioria é sobre quem responde, e o membro que volta se atualiza em vez de impor o atraso.
