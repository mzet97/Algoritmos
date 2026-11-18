---
layout: default
---

# Aula 08 (Go): pistas graduais

Volte aos [exercicios](../08.md#13-exercicios-obrigatorios) antes de abrir as solucoes.

1. O modulo nasce com `go mod init exemplo` na pasta do projeto; o `go.mod` e a identidade dele.
2. Package utilitario mora numa subpasta (`stringsx/`) e declara `package stringsx`.
3. Exportado comeca com maiuscula; o teste de acesso e um `go build` que **falha** no simbolo minusculo.
4. `go fmt ./...` formata a arvore; `go mod tidy` acerta as dependencias do `go.mod`.
