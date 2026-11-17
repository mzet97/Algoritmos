---
layout: default
---

# Aula 06 (C++): pistas graduais

Volte aos [exercicios](../06.md#20-exercicios-obrigatorios) antes de abrir as solucoes.

1. Ponteiro observador nao deleta nem prolonga vida: anote de quem e o objeto observado.
2. `unique_ptr` se move, nao se copia: a funcao recebe por valor e voce usa `std::move` na chamada.
3. Com dois `shared_ptr` do mesmo objeto, imprimam `use_count` em cada etapa.
4. `new` manual separa alocacao de construcao e deixa caminhos sem `delete`; compare com uma linha de `make_unique`.
5. Referencia, observador e dono diferem em duas perguntas: pode ser nulo? quem libera?
