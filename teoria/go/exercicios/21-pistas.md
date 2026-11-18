---
layout: default
---

# Aula 21 (Go): pistas graduais

Volte aos [exercicios](../21.md#20-exercicios-obrigatorios) antes de abrir as solucoes.

1. O identificador nasce no registro do cliente no hub — e viaja na mensagem, nao no texto do usuario.
2. Contar clientes e ler o estado do hub por um canal de consulta ou um metodo protegido.
3. Buffer `send` pequeno enche rapido: e ai que a politica para cliente lento aparece.
4. Cliente lento: tentar enviar com `select`/`default` e, no estouro, desconectar e registrar.
5. `SetReadLimit` fecha quem manda mensagem maior que o teto — teste com uma gigante.
6. Mesma `package main`, tres arquivos: hub, cliente, bootstrap/harness.
