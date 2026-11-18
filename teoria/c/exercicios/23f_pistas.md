---
layout: default
---

# Aula 23f: pistas graduais

Volte aos [exercícios](../23f_limites_globais.md#8-exercícios-obrigatórios) antes de abrir as soluções.

1. Com teto por IP em 8, a 3ª conexão do mesmo IP passa. O teste prova por-IP com exatamente 3 conexões de `127.0.0.1` esperando recusa na última: sem recusa, o `recv` do motivo expira. Diga qual linha do teste expira e por que o global de 8 não salva esse caso (são só 3 conexões).
2. Sem voltar ao `poll`, o laço de clientes visita os fds recém-aceitos com `pf[idx]` além dos consultados — `revents` é lixo de pilha. Se o lixo tiver `POLLERR`/`POLLHUP`, o inocente é fechado; a contagem corrompe e o teste expira esperando eco de quem morreu. O `continue` existe para que cliente novo só seja tocado com `revents` válido.
3. Uma linha por segundo são ~2 bytes/s por conexão: a janela de 64 KiB nunca enche, a linha tem 2 bytes, o IP tem 1 conexão. Nenhum dos três tetos mede *taxa por conexão ao longo do tempo*. O abuso passa limpo — e passar limpo é a resposta: o teto que falta é taxa sustained por conexão, que exigiria histórico por fd, não contador de janela.
4. Fechar com 146 KiB recebidos e não lidos no buffer gera RST — o kernel descarta o não lido e o motivo `CHEIO bytes` nunca chega. O teste espera a string no canal e recebe `ConnectionResetError`. Silenciar (ler-e-descartar) existe para entregar o aviso antes de qualquer punição.
