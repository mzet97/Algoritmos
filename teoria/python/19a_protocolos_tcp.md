---
layout: default
---

# Aula 19a — Protocolos TCP em Python puro: framing antes de framework

## Nesta aula

Você vai aprender a implementar framing antes de framework, tratar leitura parcial, e confrontar cada byte com o protocolo.

Você vai executar um servidor de linhas com teto de 1 024 bytes e demonstrar os cinco casos do contrato de framing: comando fragmentado em dois envios, dois comandos coalescidos num envio, comando malformado e UTF-8 inválido (ERRO sem matar o servidor), linha longa (só a conexão infratora fecha) e cliente lento sem bloquear os demais.

**Contrato e exemplo:** protocolo por linhas UTF-8 (`PING`/`ECO`/`SAIR`); `recv` devolve quantos bytes houver — nunca "uma mensagem". O [arquivo-fonte](exemplos/19a_protocolo.py) sobe o servidor em porta efêmera e roda os cinco casos como teste:

```bash
python3 teoria/python/exemplos/19a_protocolo.py
# 19a ok: fragmentado, coalescido, malformado, longo e lento isolados
```

**Leitura:** TCP é fluxo de bytes, e os dois primeiros casos são a mesma aula vista dos dois lados: `PI`+`NG\n` precisa virar um comando (buffer até o `\n`), e `PING\nECO ola\n` precisa virar dois (o resto do buffer não se joga fora). Quem escreve `recv(1024).decode().strip()` e trata como mensagem passou nos testes que escreveu e falhará com o primeiro cliente que fragmentar. Thread por conexão resolve o cliente lento neste tamanho — com o custo declarado de uma thread por cliente, que é exatamente a discussão que a aula 23 de protocolos e o servidor endurecido das outras trilhas continuam (teto de conexões, deadlines). UTF-8 inválido é erro da conexão, não do processo: `decode` com exceção capturada na fronteira, nunca `errors="ignore"` silencioso em protocolo que promete texto.

### Os cinco casos, um a um (o que cada um proíbe presumir)

Fragmentado proíbe "um `recv`, um comando": a rede fatia onde quer, e o buffer por conexão existe para remontar. Coalescido proíbe "um `recv`, no máximo um comando": o resto do buffer é o próximo comando, e descartá-lo é perder pedido silenciosamente. Malformado proíbe "cliente sempre certo": `ERRO` com a conexão viva prova que o servidor distingue pedido ruim de cliente ruim. UTF-8 inválido proíbe decodificar o fluxo: `\n` é byte (0x0A), e decodificação acontece por linha completa já enquadrada — decodificar antes de enquadrar quebra multibyte fatiado no meio. Linha longa proíbe buffer sem teto: 1 024 bytes e a conexão infratora fecha sozinha, sem levar as demais — memória do servidor decidida pelo cliente é a vulnerabilidade que o teto fecha. Cliente lento proíbe "tudo na mesma thread": thread por conexão isola o lento, e o custo (uma thread por cliente) fica declarado para a aula 23 continuar com teto e deadline.

### Leitura guiada: uma linha, cinco isolamentos

A saída inteira cabe numa frase (`fragmentado, coalescido, malformado, longo e lento isolados`) porque cada caso é um isolamento: o fragmentado isola a remontagem, o coalescido o reaproveitamento, o malformado e o UTF-8 a sobrevivência do servidor ao cliente errado, o longo o teto, o lento a independência entre conexões. Nenhum caso testa funcionalidade feliz (`PING` respondendo `PONG` não aparece) — o contrato é sobre robustez de fronteira, e a funcionalidade é presumida para que o teste meça só o que promete. Servidor que passa nos cinco e falha no feliz tem bug em outro lugar; servidor que passa no feliz e falha em qualquer um dos cinco tem bug aqui.

**Erros comuns:** um `recv` por mensagem; decodificar antes de enquadrar (o `\n` é byte, a decodificação é por linha completa); linha sem teto (memória do servidor decidida pelo cliente); `SO_REUSEADDR` esquecido em teste que sobe e desce servidor; tratar desconexão no meio da linha como comando; `errors="ignore"` no decode (silencia o byte inválido e corrompe o protocolo sem acusar — erro de fronteira se captura, nunca se varre); jogar fora a sobra do buffer após o primeiro `\n` (o segundo comando some e o cliente nunca sabe por quê).

**Exercícios:** [pistas](exercicios/19a_pistas.md) e [soluções](exercicios/19a_solucoes.md): (1) teto de conexões simultâneas com contador e recusa educada; (2) deadline por conexão (cliente que conecta e nunca manda nada); (3) o mesmo protocolo com framing binário de comprimento (4 bytes) e a comparação de onde cada um quebra; (4) o modelo de ameaças de uma página deste servidor fora de loopback.

**Checklist:** buffer por conexão com sobra reaproveitada; teto antes de decodificação completa; erro fecha a conexão, nunca o processo; os cinco casos verdes; nenhuma abstração de framework escondendo o socket.

---

### Como esta aula será avaliada

| Critério | Peso |
| -------- | ---: |
| Conceitos e vocabulário (nível 1) | 20% |
| Previsão de comportamento e diagnóstico de erros (nível 2) | 40% |
| Projeto e justificativa de decisões (nível 3) | 30% |
| Registro de comandos, saídas e interpretação | 10% |

**Aprovação:** nota mínima 7,0; pelo menos 60% dos pontos devem vir de previsão e diagnóstico (níveis 2 e 3), não apenas de código que executa.

**Para se aprofundar:**

1. *Designing Data-Intensive Applications*, Martin Kleppmann (O'Reilly, ISBN 978-1449373320); 2. Documentação do FastAPI ([fastapi.tiangolo.com](https://fastapi.tiangolo.com/)); 3. *Site Reliability Engineering* (Google, O'Reilly, ISBN 978-1491929124).
