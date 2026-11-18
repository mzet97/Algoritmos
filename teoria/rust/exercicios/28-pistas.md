---
layout: default
---

# Aula 28 (Rust): pistas graduais

Volte aos [exercicios](../28.md#exercícios-obrigatórios) antes de abrir as solucoes.

1. Gere os 50 MB uma vez, fora da medicacao; conte com `BufReader` lendo e com o mapa como fatia.
2. O iterador sobre o mapa nao copia o arquivo: conte sobre `&[u8]` mapeado.
3. io_uring em duas linhas: filas de submissao/com conclusao no kernel contra uma syscall por operacao.
