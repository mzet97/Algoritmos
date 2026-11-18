---
layout: default
---

# Aula 23g: soluções comentadas

Compare após tentar os [exercícios](../23g_tls_e_autenticacao.md#7-exercícios-obrigatórios). Experimentos sobre `teoria/cpp/exemplos/23g_tls.cpp` com `g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror -lssl -lcrypto`.

1. Previsão: **`assert` no cliente** — `impressao(s) == pin` cai antes de qualquer byte de aplicação (verificado). Um bit trocado é outra identidade: pin não admite "parecido", só igualdade total. É a cláusula (b) do TOFU executando: contatos seguintes detectam troca de identidade. Note que a negação aqui é abort, não `NEGADO` — o contrato do exemplo trata pin errado como defeito de canal (programação), não como credencial errada (protocolo): token errado volta `NEGADO`, pin errado nem conversa.

2. Diagnóstico: **`NEGADO`** (verificado: `linha crua 's3nha-da-23c': NEGADO`). O servidor compara a linha inteira com `"TOKEN " + SEGREDO` — segredo sem prefixo não casa o formato. Parse primeiro, segredo depois: o formato (`TOKEN ` + segredo + `\n`) é o contrato, e o segredo fora dele é linha qualquer. Quem "facilita" aceitando o segredo puro abre a porta para qualquer substring que o contenha.

3. Projeto: throttle com drenagem funciona — `certo, e1, e2, e3, certo → BLOQUEADO` (verificado). Duas metades, ambas obrigatórias: (a) contar `NEGADO`s e recusar a partir do 3º sem avaliar o token; (b) **ler o pedido antes de recusar**. Sem (b), o servidor fecha com o pedido do cliente não lido no socket — o kernel responde RST e o cliente lê `ECONNRESET` em vez de `BLOQUEADO` (verificado: `r = -1` no `SSL_read`, a mesma física da 23f em C, agora com outra cifra). Drenar-antes-de-recusar não é gentileza: é o que separa "aviso de limite" de "queda misteriosa". Em produção, o contador seria por IP com janela — aqui, global didático com o mesmo formato de aviso.

4. Previsão: **passa** — com a comparação apagada, até pin `"qualquer-um"` completa as duas voltas (verificado). O TLS sozinho entrega cifra (ninguém lê no fio) e forward secrecy (chave de sessão derivada por ECDHE, não transportada — vazar a chave do certificado amanhã não abre a sessão de hoje). O que ele não entrega sem âncora é **identidade**: sem pin (nem CA), o cliente aceita o certificado de quem estiver no meio. Cifra sem crachá é transporte anônimo — e transporte anônimo com `SSL_VERIFY_NONE` e sem pin é o MITM documentado como se fosse proteção.
