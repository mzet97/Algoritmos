---
layout: default
---

# Aula 16b: pistas graduais

Volte aos [exercícios](../16b_sistema_de_arquivos.md#10-exercícios-obrigatórios) antes de abrir as soluções.

1. O teste só lê de volta o que foi escrito; ele não puxa o cabo. Pense em qual leitor o teste *não* simula: o que abre o nome depois da queda de energia. Com `rename` antes do `fsync`, o nome novo existe enquanto o conteúdo ainda mora só no page cache — o teste passa e a garantia evaporou.
2. `write` em pipe retorna o que coube, não o que você pediu: com leitor dormindo, o buffer (64 KiB no Linux) enche e o retorno é parcial. Transcreva o número. O loop de `escreve_tudo` existe exatamente para esse retorno; sem ele, metade dos bytes nunca sai.
3. Desenhe os três caminhos de falha: `open` do tmp, `write`/`fsync` do conteúdo, `rename`. Em cada um, quem limpa o tmp? O `fsync` do diretório usa `open(dir, O_RDONLY | O_DIRECTORY)` + `fsync` — sem ele, o crash pode levar o `rename` embora. O teste da queda: publique v1, escreva metade de v2 sem `rename`, releia o nome final.
4. `CLOCK_MONOTONIC` ao redor do laço de 100 publicações, dividido por 100. Sem `fsync` você mede `memcpy` para o page cache; com `fsync`, o disco. A decisão segue o número: se uma publicação durável custa milissegundos, cada escrita isolada paga esse preço.
