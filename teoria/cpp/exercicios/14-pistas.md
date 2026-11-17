---
layout: default
---

# Aula 14 (C++): pistas graduais

Volte aos [exercicios](../14.md#15-exercicios-obrigatorios) antes de abrir as solucoes.

1. Thread sem `join` nem `detach` destroi o objeto `std::thread` e chama `std::terminate`.
2. No contador com mutex, proteja o incremento inteiro, nao so a leitura.
3. No atomico, o incremento e uma operacao so (`fetch_add` ou `++` atomico).
4. Data race nao e "resultado errado as vezes": releia a definicao de UB no modelo de memoria.
5. Mutex protege uma regiao com invariante; atomico protege uma variavel com operacoes indivisiveis.
