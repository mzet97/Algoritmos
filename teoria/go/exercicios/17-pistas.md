---
layout: default
---

# Aula 17 (Go): pistas graduais

Volte aos [exercicios](../17.md#14-exercicios-obrigatorios) antes de abrir as solucoes.

1. Benchmark mede a funcao, nao o programa: isole o que voce quer comparar.
2. `B/op` e `allocs/op` contam o preco por operacao — e onde a refatoracao ataca.
3. Concatenar com `+` em laco copia a string inteira a cada volta; o Builder acumula.
4. `make([]T, 0, esperado)` evita realocacoes quando o tamanho e conhecido.
5. No escape, a causa manda: quem guarda o endereco depois que a funcao volta?
