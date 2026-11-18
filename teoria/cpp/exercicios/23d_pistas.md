---
layout: default
---

# Aula 23d: pistas graduais

Volte aos [exercícios](../23d_limites_globais.md#8-exercícios-obrigatórios) antes de abrir as soluções.

1. `liberar` decrementa `abertas` sem checar: −1 num orçamento vazio. Uma vaga fantasma que admite além da capacidade real. A guarda (`abertas > 0` + IP conhecido) é o par do `fecha` com guarda da 23f em C: contador que só anda para um lado mente.
2. `921600 + SIZE_MAX` estoura `size_t` e volta a `921599` — menor que 1 MiB, aceito. O teste é `soma > teto`, e soma que dá a volta perde. A forma segura compara sem somar: `n > max - atual`. Teto com aritmética que dá a volta é porta aberta com placa de "fechado".
3. Os três tetos medem ocupação (conexões), volume (bytes) e identidade (por IP) — nenhum mede *ritmo de chegada*. Reconexão em rajada do mesmo IP passa nos três se houver vaga. O quarto teto conta admissões por janela: 2 entram, a 3ª espera a próxima. Janela nova zera o contador, como nos bytes.
4. 2 MiB > 1 MiB em qualquer janela: recusado para sempre, por desenho. Teto de janela não é "até 1 MiB por pedido", é "até 1 MiB somado" — pedido maior que o teto nunca cabe na soma. Carga grande se negocia em pedaços (cada um cobrado), não num pedido que o teto jamais admitiria.
