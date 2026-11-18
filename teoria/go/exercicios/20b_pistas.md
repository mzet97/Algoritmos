---
layout: default
---

# Aula 20b: pistas graduais

Volte aos [exercícios](../20b_tls_e_ssrf.md#6-exercícios-obrigatórios) antes de abrir as soluções.

1. O pin é comparado byte a byte com o SHA-256 do certificado apresentado — um bit diferente já é outro pin. A pergunta é se o programa chega a montar o pedido HTTP: olhe o `st` do `panic`. (Verificado: `pin divergiu`, `st=0`.)
2. O servidor responde o mesmo status e o mesmo corpo nos dois casos — de propósito. O nome da propriedade tem "negação" no meio e aparece na 23g de C++. (Verificado: `401 NEGADO` nas duas linhas.)
3. Só um dos dois depende do predicado removido; o outro cai num predicado que já existia. Rode com cada alvo isolado e leia qual `motivo` o guarda imprime — ou a ausência dele. (Verificado: sem `IsUnspecified`, `0.0.0.0` viaja até `dial tcp`; `::ffff:127.0.0.1` continua bloqueado.)
4. Com `false`, a verificação normal contra as CAs do sistema volta a valer — e o certificado é autofirmado. O callback continua existindo, mas quem aborta primeiro? Leia a mensagem: ela cita pin ou cadeia? (Verificado: `x509: certificate signed by unknown authority`.)
