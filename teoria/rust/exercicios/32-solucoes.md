---
layout: default
---

# Aula 32 bônus (Rust): solucoes comentadas

Compare apos tentar o [checklist de dominio](../32_bonus.md#checklist-de-domínio). Estado executado: o projeto [32_mini_redis_avancado](../projetos/README.md) e verificado pelo `teoria/verificar_projetos.py` com clientes reais:

- `INCR` concorrente: dois clientes × 250 incrementos na mesma chave terminam em **exatamente 500** — a atomicidade por chave do `entry` do DashMap segurada sob disputa, nao so em sequencia.
- `INCR` em texto devolve `-ERR valor nao e um inteiro ou fora do intervalo`; `INCR` em `i64::MAX` devolve `-ERR incremento causaria overflow` (o `checked_add` da aula).
- TTL: `SET sessao abc EX1` responde `+abc` antes de 1s e `$-1` depois (expiracao preguicosa conferida na leitura).
- `SET` com `EX`/`PX` e chave existente substituindo valor e prazo, como o Redis real.

O que o checklist cobra e o que continua aberto, honestamente:

| Item do checklist | Estado |
| --- | --- |
| `INCR` atomico sob concorrencia | Executado (500/500) |
| TTL preguicoso + varredura | Executado o preguicoso; o varredor periodico esta no codigo da aula |
| Erros de tipo e overflow | Executados |
| AOF/RDB (persistencia) | So o desenho da aula: nao implementado no projeto |
| `MULTI`/`EXEC`, `WATCH` | Fora do escopo: transacoes nao implementadas |
| Replicacao | Fora do escopo |

Persistencia e transacoes sao os proximos degraus nomeados, na mesma ordem da aula: primeiro AOF (append-only com `fsync` por politica declarada), depois snapshots; transacoes so depois que o modelo de erros estiver estavel. Nenhum deles pode ser marcado como feito a partir desta pagina — o que esta feito e medido e a tabela acima.

**Autoverificacao:** os 500 incrementos conferidos no verificador, o overflow recusado sem alterar o valor e a frase "TTL preguicoso so percebe na proxima leitura" explicada com o experimento do `EX1`.
