---
layout: default
---

# Aula 32 bônus (Rust): pistas graduais

Volte ao [checklist](../32_bonus.md#checklist-de-domínio) antes de abrir as solucoes. A solucao executada e o projeto [32_mini_redis_avancado](../projetos/README.md), verificado por `teoria/verificar_projetos.py`.

1. `INCR` atomico por chave mora no `entry` do DashMap: ninguem mais escreve naquela chave entre ler e somar.
2. TTL preguicoso so percebe a expiracao quando alguem toca a chave; o varredor em segundo plano e o outro mecanismo.
3. Bulk string nula (`$-1`) e a resposta de ausencia/expiracao; erro de tipo e `-ERR` com texto.
