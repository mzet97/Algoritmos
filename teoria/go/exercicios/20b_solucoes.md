---
layout: default
---

# Solucoes comentadas da aula 20b

1. Com o pin corrompido, o cliente aborta no handshake: `pin divergiu`, `st=0` — nenhum byte HTTP viaja, o token nunca sai do processo. A falha acontece na camada TLS (callback `VerifyPeerCertificate`), antes de qualquer pedido: pinar é autenticar o par antes de falar com ele, e "antes" é a garantia.
2. Token errado e ausente devolvem `401 NEGADO` idênticos — negação única. Se "ausente" devolvesse outro corpo ou status, o atacante teria um oráculo de formato: sondaria quais requisições carregam token sem precisar acertá-lo. A uniformidade fecha esse oráculo (mesma decisão da 23g de C++): para quem não tem o segredo, os dois casos são o mesmo caso.
3. Sem `|| ip.IsUnspecified()`, `http://0.0.0.0/` viaja até `dial tcp 0.0.0.0:80` (recusado só porque nada escuta — numa máquina com serviço local, buscaria o localhost). `http://[::ffff:127.0.0.1]/` continua bloqueado, porque `IsLoopback` enxerga através do mapeamento IPv6 (`ParseIP` + `To4` interno: só `IsUnspecified` é verdadeiro para `0.0.0.0`, só `IsLoopback` para o mapeado — medido). O curinga exigiu o predicado que o ramo de nomes já tinha (`::` cai no mesmo `IsUnspecified`): o ramo literal foi escrito antes do de nomes e ficou para trás — assimetria honesta, agora corrigida no exemplo.
4. Com `InsecureSkipVerify: false`, o handshake morre com `x509: certificate signed by unknown authority`: a verificação normal contra as CAs do sistema roda e reprova o autofirmado, e o callback de pin não a socorre — quem decide é a cadeia, não o pin. A lição é a do erro nº 1 da aula, pelo avesso: `true` + callback = "a CA sou eu, e eu verifico exatamente isto"; `false` + callback = "a CA do sistema decide, e meu pin é opinião". TOFU honesto exige o primeiro.
