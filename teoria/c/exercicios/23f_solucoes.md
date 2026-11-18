---
layout: default
---

# Aula 23f: soluções comentadas

Compare após tentar os [exercícios](../23f_limites_globais.md#8-exercícios-obrigatórios). Servidor em `teoria/c/exemplos/23f_limites.c`, teste em `teoria/c/exemplos/test_23f.py`; variantes compiladas com as mesmas flags da aula.

1. Previsão: com `TETO_POR_IP = 8`, o primeiro comportamento a quebrar é o **por-ip** — a 3ª conexão de `127.0.0.1` é aceita em vez de recusada, e o teste expira no `recv` do motivo (`test_23f.py`, fase por-ip: `TimeoutError` em vez de `CHEIO por-ip`). O global de 8 não salva: são só 3 conexões. A aritmética dos aliases (`127.0.0.2`–`.4`) existia para provar o global *com* o por-ip em 2; com por-ip em 8, um IP só encheria o global antes — e o teste deixaria de provar os dois tetos separadamente.

2. Diagnóstico: sem o `continue` pós-dreno, o teste falha com `TimeoutError` (verificado). O laço de clientes percorre `pf[idx]` para fds que o `poll` desta iteração não consultou — `revents` ali é lixo de pilha. Quando o lixo contém `POLLERR`/`POLLHUP`/`POLLNVAL`, o recém-aceito é fechado no nascimento; a contagem corrompe e ecos esperados nunca chegam. O `continue` (`if (aceitou) continue;`) garante: cliente novo só é tocado na iteração seguinte, com `revents` válido.

3. Projeto: o gotejamento (1 linha/s, verificado: 3 ecos `ECO x` limpos, nenhum aviso) passa por todos os tetos. Janela de 64 KiB/s nunca enche com ~2 bytes/s; linha de 1023 nunca vê 2 bytes como longa; 1 conexão nunca atinge teto por IP nem global. O teto que falta é **taxa sustentada por conexão** — histórico por fd (tokens por segundo), não contador de janela global. Nenhum dos três existentes o pegaria porque nenhum deles mede taxa por conexão ao longo do tempo.

4. Previsão: `ConnectionResetError`, não `CHEIO bytes` (verificado: `parte = rajada.recv(4096)` levanta `[Errno 104] Connection reset by peer`). Com ~146 KiB recebidos e não lidos no buffer, `close` responde RST e o motivo se perde — testado empiricamente na aula inclusive com `SO_LINGER`, que não adianta. Silenciar (ler-e-descartar até a janela virar, avisando no canal) existe para entregar o aviso antes de qualquer punição: throttle com aviso é protocolo, fechar com RST é punição sem intimação.
