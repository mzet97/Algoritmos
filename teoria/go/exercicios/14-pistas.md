---
layout: default
---

# Aula 14 (Go): pistas graduais

Volte aos [exercicios](../14.md#13-exercicios-obrigatorios) antes de abrir as solucoes.

1. O teste mora em `pacote_test.go` no mesmo diretorio e chama a funcao como qualquer cliente.
2. `-v` mostra cada teste pelo nome; sem ele, so o veredito do pacote.
3. Benchmark e funcao `BenchmarkX(b *testing.B)` com o laco em `b.N`.
4. Cobertura se le como mapa de por onde os testes passaram, nao como nota.
5. `go vet` acusa construcoes suspeitas que compilam (printf com verbo errado e o classico).
