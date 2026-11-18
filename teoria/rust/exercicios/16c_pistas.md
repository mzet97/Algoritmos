---
layout: default
---

# Aula 16c: pistas graduais

Volte aos [exercícios](../16c_isa_e_abi.md#6-exercícios-obrigatórios) antes de abrir as soluções.

1. Com `repr(Rust)`, `etiqueta` foi parar no offset **12** (verificado: o `assert` de offset 0 cai). O compilador reordenou para compactar/otimizar — direito dele, sem `repr(C)`. Contrato vs. acidente: `repr(C)` promete ordem e deslocamentos; `repr(Rust)` promete nada; "passa hoje" sem promessa é coincidência compilada.
2. Com `read`, passa no x86-64 (verificado). O contrato de `read` exige endereço alinhado; a ISA x86 tolera o desalinhado em silêncio. Verde aqui prova a tolerância do hardware, não a correção do programa — em ARM, falha; sob Miri, `misaligned` sempre. UB que funciona é o pior UB: o teste mente e a mentira viaja para a plataforma seguinte.
3. Com `sinal: u16` depois de `comprimento`: offsets (0, 4, 8, 16), tam 24, alin 8 (verificado). `etiqueta` ocupa 0, padding 1–3, `comprimento` em 4–7, `sinal` em 8–9, padding 10–15, `total` em 16–23. Cada padding alinha o próximo campo ao seu tamanho; o último arredonda o tamanho ao alinhamento. Com `repr(Rust)`, essa conta seria chute — o compilador não deve explicações.
4. `soma_c(u32::MAX, 1)` devolve **0**, sem pânico (verificado). `wrapping_add` define o estouro (volta a zero) em vez de deixar para o otimizador — que, no modo `release` sem `wrapping`, assumiria que estouro "não acontece" e otimizaria em cima da suposição. Fronteira com C usa aritmética com estouro definido porque do outro lado não há `panic` nem `checked`: há byte, e byte dá a volta.
