---
layout: default
---

# Aula 15a: soluções comentadas

Compare após tentar os [exercícios](../15a_auditoria_unsafe.md#7-exercícios-obrigatórios). Experimentos sobre `teoria/rust/exemplos/15a_auditoria.rs` com `rustc --edition 2024 -O -D warnings`.

1. Previsão: **pânico** — `index out of bounds: the len is 2 but the index is 2` (verificado). Mas atenção ao que o pânico prova: com a pilha cheia, o índice cai fora e o acesso *safe* barra antes do `unsafe`. Com a pilha parcial (`tamanho = 1`), `dados[1]` estaria dentro dos limites — e leria lixo em silêncio, UB sem sintoma. O pânico observado é sorte do preenchimento, não segurança do código: invariante quebrada nem sempre avisa, e auditoria não conta com aviso.

2. Diagnóstico: o 3º `push` morre no mesmo pânico de índice (verificado) — antes de escrever. O `write` (o `unsafe` de verdade) nem executou: a verificação de limites do acesso seguro barrou primeiro. A checagem `cheia` é o contrato (recusa sem perda, devolve o valor no `Err`); sem ela, o backstop vira plano A — e backstop só barra o que está fora dos limites. Dentro, é com a invariante.

3. Projeto: `espiar` com `assume_init_ref` no slot `tamanho - 1` (verificado: `None` vazia, `Some("a")` sem remover, `pop` depois continua íntegro). A invariante é a do `pop` (slot inicializado) mais uma: só empresta — não move, não invalida, não estende a vida além do próximo `&mut`. O perigo novo: guardar o `&T`, chamar `pop` + `push`, e ler a referência pendurada no valor novo (ou no lixo). `&T` não move, mas prende tempo de vida; auditoria cobra o empréstimo junto com o slot.

4. Previsão: **5 Drops em vez de 2** (verificado). Descartar `0..N` executa `Drop` nos slots não-inicializados: cada lixo "morrido" roda código de destruição em bytes quaisquer — com o contador, +3 fantasmas; com `String`, double-free ou abort. O `Drop` que não respeita `tamanho` quebra a invariante mais importante da estrutura (só o inicializado morre) — e a auditoria cobra o `Drop` com número exatamente para que essa quebra apareça em dígito, não em coredump intermitente.
