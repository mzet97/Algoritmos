---
layout: default
---

# Aula 10 (Go): pistas graduais

Volte aos [exercicios](../10.md#13-exercicios-obrigatorios) antes de abrir as solucoes.

1. Canal sem buffer sincroniza: o envio so completa quando alguem recebe.
2. Com buffer de 1, o envio completa antes da leitura — observe a ordem das impressoes.
3. No `select` entre dois canais, o caso pronto primeiro vence; prontos juntos, a escolha e pseudoaleatoria.
4. `time.After` e um canal que entrega uma vez so, depois do intervalo.
5. Channel contra variavel global: quem espera pelo dado sem ficar girando (busy wait)?
