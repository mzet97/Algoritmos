---
layout: default
---

# Soluções comentadas da aula 23a

## 1. Caminho do `PING` com leitor lento ao lado

`poll` acorda com `POLLIN` no fd do PING e nada no fd do lento (que não enviou nada novo). `recv` lê o que houver; `consumir_linhas` responde `PONG`; `POLLOUT` drena a resposta; o lento continua com a dele pendente. `EAGAIN` em qualquer ponto significa "volte depois", nunca "feche" — fechar acontece só em erro real, protocolo violado, teto ou ociosidade.

## 2. Teto de 64 bytes

A primeira resposta (`PONG\n`, 5 bytes) cabe; o teste enche a fila e o `enfileirar` recusa a próxima: conexão derrubada, slot liberado. Os demais nem percebem. Sem teto, esse cliente cresceria a fila sem limite — vazamento com formato de protocolo.

## 3. 300 conexões contra teto de 200

Parte cai no `backlog` do `listen` (`ECONNREFUSED` no cliente), parte é aceita e imediatamente fechada pelo teto (EOF limpo, pois nada foi lido ainda), e as demais operam normal. Conte cada grupo: backlog é fila do SO, teto é política do programa, `ulimit -n` é o teto do processo. Três limites diferentes, três culpados diferentes.

## 4. Linha gigante sem terminador

Com `\n` no fim, o servidor lê, encontra o teto e derruba (EOF ou RST — ambos sem resposta). Sem `\n` e segurando a conexão, o servidor acumula até `ENTRADA_MAX` e derruba pelo mesmo caminho. Se o cliente enviar de conta-gotas para nunca encher o buffer, quem derruba é o timeout de ociosidade (10 s padrão) — cada caminho de entrada tem um teto, cada espera tem um relógio.
