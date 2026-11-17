---
layout: default
---

# Aula 11 (C++): pistas graduais

Volte aos [exercicios](../11.md#15-exercicios-obrigatorios) antes de abrir as solucoes.

1. Nao achou nao e excecao nem valor sentinela: `std::nullopt` representa ausencia no tipo.
2. No parse com `std::expected`, o erro e um valor que o chamador e obrigado a olhar.
3. No `variant`, `std::visit` com lambdas sobrecarregadas cobre todos os casos — esqueceu um, nao compila.
4. Excecao vence quando o erro e raro, atravessa muitas camadas e quem pode tratar esta longe.
5. RAII com excecao: o desenrolar da pilha destroi os objetos do caminho — quem libera e o destrutor, nao um `catch`.
