---
layout: default
---

# Aula 22 (Go): pistas graduais

Volte aos [exercicios](../22.md#21-exercicios-obrigatorios) antes de abrir as solucoes.

1. `QueryRowContext` + `Scan`: zero linhas nao e erro de SQL, e `sql.ErrNoRows`.
2. Liste com `QueryContext`, `defer rows.Close()` e `rows.Err()` depois do laco.
3. Mapeie `ErrNoRows` para um erro do seu dominio com `errors.Is` — o handler decide o 404.
4. Transacao junta o que precisa nascer junto: `BeginTx`, tudo ou nada, `defer tx.Rollback()`.
5. Pool pequeno demais serializa; grande demais briga com o banco. SQLite local aceita 1 escritor.
6. N+1: uma consulta por item de uma lista — some com um `JOIN` ou um `IN`.
