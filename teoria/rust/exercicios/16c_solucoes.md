---
layout: default
---

# Aula 16c: soluções comentadas

Compare após tentar os [exercícios](../16c_isa_e_abi.md#6-exercícios-obrigatórios). Experimentos sobre `teoria/rust/exemplos/16c_isa_abi.rs` com `rustc --edition 2024 -O -D warnings` (Miri indisponível nesta máquina — o exercício 2 diz onde ele entraria).

1. Previsão: **quebra** — com `repr(Rust)`, `etiqueta` foi para o offset **12** (verificado: `left: 12, right: 0`). O compilador reordenou os campos (provavelmente `comprimento`, `total`, `etiqueta` com outro padding) — direito documentado sem `repr(C)`. "Passava com `repr(C)`" era contrato cumprido; "quebrou com `repr(Rust)`" é liberdade exercida. Quem lê layout sem `repr(C)` lê implementação, não promessa — e implementação de compilador muda entre versões.

2. Diagnóstico: **passa** no x86-64 (verificado: `0x04030201` intacto). O contrato de `read` exige alinhamento; a ISA x86 tolera o desalinhado e entrega o valor certo. Verde aqui prova a tolerância do hardware — em ARM, o mesmo programa falha; sob Miri (`cargo miri run`, componente a instalar), acusa `misaligned pointer` sempre. UB que funciona é o pior UB porque o teste mente: viaja para a próxima plataforma como "código que passava".

3. Projeto: com `sinal: u16` após `comprimento`: offsets **(0, 4, 8, 16)**, tam **24**, alin **8** (verificado). Conta: `etiqueta` em 0, padding 1–3 (alinhar o `u32`), `comprimento` em 4–7, `sinal` em 8–9, padding 10–15 (alinhar o `u64`), `total` em 16–23; tamanho arredondado ao alinhamento (24). Cada padding existe para um campo; o último, para o array. Com `repr(Rust)`, essa conta seria chute em duas direções (ordem + padding) — o compilador não deve explicações, e o exercício prova pedindo a previsão antes da execução.

4. Previsão: **0, sem pânico** (verificado). `wrapping_add` define o estouro (volta a zero) em qualquer perfil — inclusive `release`, onde aritmética comum assume "estouro não acontece" e o otimizador decide o resto. Fronteira com C usa estouro definido porque do outro lado não há `panic`, `checked` nem `overflow-checks`: há bytes, e bytes dão a volta. O contrato da fronteira diz o que acontece no limite; sem ele, quem decide é o nível de otimização — e otimização não é semântica.
