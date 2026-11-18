---
layout: default
---

# Aula 13 (Rust): pistas graduais

Volte aos [exercicios](../13.md#15-exercícios-obrigatórios) antes de abrir as solucoes.

1. Soma paralela: reparta em fatias com `chunks`, cada thread devolve a soma parcial por `join`.
2. Pipeline: canal que fecha quando o remetente cai — o fim do gerador encerra todo mundo.
3. Nas tres formas do contador, o que muda e onde o resultado parcial vive.
4. `sync_channel(2)` bloqueia o produtor no `send` quando cheio; imprima antes/depois.
5. MapReduce: cada chunk vira um `HashMap` parcial; o reduce soma mapas, nao textos.
6. RwLock contra Mutex: muitos leitores simultaneos so no primeiro; meça com a mesma carga.
7. Pool: fila compartilhada com `Arc<Mutex<Receiver>>` ou crossbeam — roubos exigem mais que um mutex global.
