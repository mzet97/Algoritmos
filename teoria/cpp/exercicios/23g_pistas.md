---
layout: default
---

# Aula 23g: pistas graduais

Volte aos [exercícios](../23g_tls_e_autenticacao.md#7-exercícios-obrigatórios) antes de abrir as soluções.

1. Um bit trocado é outra impressão: `assert(impressao(s) == pin)` cai no cliente, antes de qualquer byte de aplicação. Pin não mede "parecença" — igualdade total ou negação. É a cláusula (b): contatos seguintes detectam troca de identidade; atacante com outro certificado gera outra impressão e para aqui.
2. O servidor compara a linha inteira com `"TOKEN " + SEGREDO`: segredo sem prefixo é só uma linha que não casa — `NEGADO`. O formato é o contrato: o segredo fora do formato não é "quase certo", é outro pedido. Parse primeiro, segredo depois.
3. Contador de `NEGADO`s no servidor; a partir de 3, responde `BLOQUEADO` sem avaliar. A armadilha: escrever `BLOQUEADO` sem ler o pedido fecha com dado não lido no socket — RST, e o cliente lê `ECONNRESET` em vez do motivo (a mesma física da 23f, com outra cifra). Drena a linha primeiro, recusa depois: throttle com aviso é protocolo.
4. Sem comparar, qualquer pin passa — inclusive `"qualquer-um"` (verificado: as duas voltas passam). O TLS sozinho entrega cifra e forward secrecy (chave efêmera derivada, não transportada); identidade ele não entrega sem âncora — sem pin nem CA, o túnel é anônimo e aceita o certificado de quem estiver no meio. Cifra sem crachá é transporte, não autenticação.
