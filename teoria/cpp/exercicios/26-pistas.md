---
layout: default
---

# Aula 26 (C++): pistas graduais

Volte aos [exercicios](../26.md#21-exercicios-obrigatorios) antes de abrir as solucoes.

1. Na conversao: o header fica so com a declaracao de `struct Impl` e o `unique_ptr<Impl>`; tudo que incluia o mundo vai para o `.cpp`.
2. Recompilacao em cascata nasce de header que muda; se o header para de mudar, quem incluiu nao recompila.
3. ABI estavel pede layout publico estavel; PIMPL esconde o layout, mas vtables, excecoes e alocador continuam fronteira.
4. PIMPL e exagero quando a classe e pequena, interna e muda junto com todos os clientes.
5. `unique_ptr<Impl>` exige destrutor declarado no header e definido no `.cpp` — descubra por que.
