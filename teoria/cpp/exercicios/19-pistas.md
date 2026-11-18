---
layout: default
---

# Aula 19 (C++): pistas graduais

Volte aos [exercicios](../19.md#19-exercicios-obrigatorios) antes de abrir as solucoes.

1. Ownership se descreve recurso a recurso: quem cria, quem destroi, quem pode emprestar.
2. Politica de erro coerente escolhe **um** mecanismo primario por camada e diz o que nunca acontece (excecao escapando, erro ignorado).
3. Na CLI, desenhe os tres canais antes das opcoes: dado em `stdout`, diagnostico em `stderr`, contrato no exit code.
4. API grande demais: conte quantas decisoes o cliente precisa tomar antes do primeiro uso util.
5. O fluxo minimo tem tres sabores do mesmo fonte: debug com sanitizers, teste automatizado e release otimizado sem sanitizer.
