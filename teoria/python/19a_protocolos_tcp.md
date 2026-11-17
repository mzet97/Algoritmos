---
layout: default
---

# Aula 19a — Protocolos TCP em Python puro: framing antes de framework

**Competências SDD:** NET, CON, SEC. **Pré-requisitos:** 13 (I/O) e [16b](16b_deadlock_e_backpressure.md). Esta é a ponte do PY6: socket, framing e entrada hostil com a biblioteca padrão, **antes** de qualquer FastAPI das aulas 20+.

**Objetivo observável:** executar um servidor de linhas com teto de 1 024 bytes e demonstrar os cinco casos do contrato de framing: comando fragmentado em dois envios, dois comandos coalescidos num envio, comando malformado e UTF-8 inválido (ERRO sem matar o servidor), linha longa (só a conexão infratora fecha) e cliente lento sem bloquear os demais.

**Contrato e exemplo:** protocolo por linhas UTF-8 (`PING`/`ECO`/`SAIR`); `recv` devolve quantos bytes houver — nunca "uma mensagem". O [arquivo-fonte](exemplos/19a_protocolo.py) sobe o servidor em porta efêmera e roda os cinco casos como teste:

```bash
python3 teoria/python/exemplos/19a_protocolo.py
# 19a ok: fragmentado, coalescido, malformado, longo e lento isolados
```

**Leitura:** TCP é fluxo de bytes, e os dois primeiros casos são a mesma aula vista dos dois lados: `PI`+`NG\n` precisa virar um comando (buffer até o `\n`), e `PING\nECO ola\n` precisa virar dois (o resto do buffer não se joga fora). Quem escreve `recv(1024).decode().strip()` e trata como mensagem passou nos testes que escreveu e falhará com o primeiro cliente que fragmentar. Thread por conexão resolve o cliente lento neste tamanho — com o custo declarado de uma thread por cliente, que é exatamente a discussão que a aula 23 de protocolos e o servidor endurecido das outras trilhas continuam (teto de conexões, deadlines). UTF-8 inválido é erro da conexão, não do processo: `decode` com exceção capturada na fronteira, nunca `errors="ignore"` silencioso em protocolo que promete texto.

**Erros comuns:** um `recv` por mensagem; decodificar antes de enquadrar (o `\n` é byte, a decodificação é por linha completa); linha sem teto (memória do servidor decidida pelo cliente); `SO_REUSEADDR` esquecido em teste que sobe e desce servidor; tratar desconexão no meio da linha como comando.

**Exercícios:** [pistas](exercicios/19a_pistas.md) e [soluções](exercicios/19a_solucoes.md): (1) teto de conexões simultâneas com contador e recusa educada; (2) deadline por conexão (cliente que conecta e nunca manda nada); (3) o mesmo protocolo com framing binário de comprimento (4 bytes) e a comparação de onde cada um quebra; (4) o modelo de ameaças de uma página deste servidor fora de loopback.

**Checklist:** buffer por conexão com sobra reaproveitada; teto antes de decodificação completa; erro fecha a conexão, nunca o processo; os cinco casos verdes; nenhuma abstração de framework escondendo o socket.
