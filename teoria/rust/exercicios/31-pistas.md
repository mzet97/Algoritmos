---
layout: default
---

# Aula 31 (Rust): pistas graduais

Volte aos [exercicios](../31.md#checklist) antes de abrir as solucoes. A solucao executada desta aula e o projeto [31_mini_redis](../projetos/README.md), verificado por `teoria/verificar_projetos.py`.

1. Testar na mao e `nc 127.0.0.1 6379` (ou `telnet`): um comando por linha, resposta por linha.
2. Dois terminais ao mesmo tempo provam concorrencia; a serializacao por shard aparece so sob carga disputada.
3. `DEL` de chave inexistente devolve `:0` — politica declarada, nao erro.
