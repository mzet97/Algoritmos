---
layout: default
---

# Aula 24 (C++): pistas graduais

Volte aos [exercicios](../24.md#21-exercicios-obrigatorios) antes de abrir as solucoes.

1. A troca para `pmr` muda os tipos (`std::pmr::vector<std::pmr::string>`) e pede um resource no construtor.
2. Parsing por request nasce e morre junto: arena monotonica aloca rapido e libera tudo de uma vez.
3. Pool faz sentido quando blocos de tamanhos parecidos sao pedidos e devolvidos fora de ordem, muitas vezes.
4. Objeto `pmr` que sobrevive ao resource que o alimenta aponta para memoria reciclada.
5. Antes de pmr: onde esta a prova de que alocacao e gargalo, e quem e o dono de cada resource?
