---
layout: default
---

# Aula 18 (Go): pistas graduais

Volte aos [exercicios](../18.md#13-exercicios-obrigatorios) antes de abrir as solucoes.

1. cgo faz sentido onde o C ja e a realidade: biblioteca madura sem equivalente, hardware, legado.
2. Ownership na fronteira: memoria alocada em C se libera com `C.free`, nunca com suposicoes do GC.
3. Custos operacionais comecam no build: compilador C, cross-compilation quebrada, binario que ja nao e estatico puro.
4. Antes de cgo, esgote a biblioteca padrao e o ecossistema Go puro — a pesquisa e parte do exercicio.
5. Portabilidade: Go puro entrega um binario por `GOOS/GOARCH`; com cgo, cada alvo pede seu toolchain C.
