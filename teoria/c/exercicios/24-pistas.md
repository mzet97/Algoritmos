---
layout: default
---

# Aula 24: pistas graduais

Volte aos [exercicios](../24.md#21-exercicios-obrigatorios) antes de abrir as solucoes.

1. Troque a formula de crescimento e conte duas coisas ao empurrar N elementos: quantas vezes realocou e quanto da capacidade fica ociosa no fim.
2. Fragmentacao interna mora dentro do bloco alocado; a externa, nos buracos entre blocos que somam o bastante mas nao servem juntos.
3. Sinais de alocacao excessiva aparecem no perfil: tempo em `malloc/free`, RSS que cresce sem a carga crescer e latencia com pausas de realocacao.
4. Misturar vidas: o objeto longo vira obstaculo entre objetos curtos que ja morreram.
5. Capacidade extra troca memoria previsivel por menos realocacoes; quantifique o preco com a formula de crescimento.
