---
layout: default
---

# Aula 15a: pistas graduais

Volte aos [exercícios](../15a_auditoria_unsafe.md#7-exercícios-obrigatórios) antes de abrir as soluções.

1. Com a pilha cheia, `dados[2]` em len 2: pânico de índice (verificado). Mas repare: o pânico veio da verificação de limites do acesso *safe* — se a pilha estivesse parcial (`tamanho = 1`), `dados[1]` estaria dentro dos limites e leria lixo em silêncio. Invariante quebrada nem sempre avisa; quando avisa, é o backstop, não a prova.
2. O 3º `push` escreve `dados[2]` e o índice verificado barra antes da escrita (verificado: pânico). O `unsafe` (`write` no slot) nem executou — a checagem `cheia` é lógica do contrato (recusa sem perda, devolvendo o valor), e o pânico é o que acontece quando o contrato falta. `unsafe` sem invariante escrita é onde o backstop vira plano A.
3. `assume_init_ref` no slot `tamanho - 1`, com `None` quando vazia: `espiar` vê sem remover (verificado). O cuidado é o mesmo do `pop` — o slot precisa estar inicializado pela invariante — mais um: a referência não pode sobreviver a `pop`+`push` no mesmo slot (empréstimo pendurado). `&T` não move, mas prende; auditoria cobra o tempo de vida junto.
4. Descartar `0..N` executa `Drop` em lixo: a contagem sai **5** em vez de 2 (verificado). Cada slot não-inicializado "descartado" roda `Drop` em bytes quaisquer — com `String`, double-free; com contador, número errado. O `Drop` que não respeita `tamanho` transforma a invariante mais importante (só o inicializado morre) em sugestão.
