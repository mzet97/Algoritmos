---
layout: default
---

# Aula 32 (C++): pistas graduais

Volte aos [exercicios](../32.md#18-exercicios-obrigatorios) antes de abrir as solucoes.

1. `remove` devolve se removeu ou se a chave nao existia — escolha e documente; nada de excecao para ausencia comum.
2. Carregar do arquivo e o mesmo laco da [aula 16](16-solucoes.md): linha, primeiro `=`, politicas declaradas.
3. O teste de ausencia cobre obter e remover o que nunca existiu.
4. Logging por callback recebe nivel e mensagem; a loja nao conhece `std::cerr`.
5. Copiar as strings na entrada desacopla a loja do tempo de vida dos buffers do chamador.
6. Evoluir sem quebrar: a API publica fica, a estrategia interna (vetor → hash) troca atras dela.
