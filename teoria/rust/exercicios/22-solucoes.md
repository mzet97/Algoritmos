---
layout: default
---

# Aula 22 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../22.md#exercícios-obrigatórios). Projeto executado nesta rodada com SQLite local (sem servidor): `cargo add sqlx --features runtime-tokio,sqlite tokio --features full`. Estrutura: `migrations/0001_contas.sql`, `src/main.rs`.

```sql
-- migrations/0001_contas.sql
CREATE TABLE IF NOT EXISTS contas (
    id INTEGER PRIMARY KEY,
    titular TEXT NOT NULL,
    saldo_centavos INTEGER NOT NULL CHECK (saldo_centavos >= 0)
);

INSERT OR IGNORE INTO contas (id, titular, saldo_centavos) VALUES
    (1, 'Ana', 10000),
    (2, 'Bruno', 5000);
```

```rust
use sqlx::sqlite::SqlitePoolOptions;
use sqlx::SqlitePool;

async fn saldo(pool: &SqlitePool, id: i64) -> Result<i64, sqlx::Error> {
    let (saldo,): (i64,) = sqlx::query_as("SELECT saldo_centavos FROM contas WHERE id = ?")
        .bind(id)
        .fetch_one(pool)
        .await?;
    Ok(saldo)
}

async fn transferencia(pool: &SqlitePool, de: i64, para: i64, valor: i64) -> Result<(), String> {
    if valor <= 0 {
        return Err(String::from("valor deve ser positivo"));
    }
    let mut tx = pool.begin().await.map_err(|e| e.to_string())?;
    let saldo_origem: (i64,) = sqlx::query_as("SELECT saldo_centavos FROM contas WHERE id = ?")
        .bind(de)
        .fetch_one(&mut *tx)
        .await
        .map_err(|e| e.to_string())?;
    if saldo_origem.0 < valor {
        return Err(String::from("saldo insuficiente")); // tx cai sem commit: rollback
    }
    sqlx::query("UPDATE contas SET saldo_centavos = saldo_centavos - ? WHERE id = ?")
        .bind(valor)
        .bind(de)
        .execute(&mut *tx)
        .await
        .map_err(|e| e.to_string())?;
    sqlx::query("UPDATE contas SET saldo_centavos = saldo_centavos + ? WHERE id = ?")
        .bind(valor)
        .bind(para)
        .execute(&mut *tx)
        .await
        .map_err(|e| e.to_string())?;
    tx.commit().await.map_err(|e| e.to_string())?;
    Ok(())
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let pool = SqlitePoolOptions::new()
        .connect("sqlite://banco.db?mode=rwc")
        .await?;
    sqlx::migrate!("./migrations").run(&pool).await?;

    println!("antes: Ana={} Bruno={}", saldo(&pool, 1).await?, saldo(&pool, 2).await?);
    transferencia(&pool, 1, 2, 3000).await.map_err(|e| -> Box<dyn std::error::Error> { e.into() })?;
    println!("depois de 3000: Ana={} Bruno={}", saldo(&pool, 1).await?, saldo(&pool, 2).await?);

    let tentativa = transferencia(&pool, 1, 2, 999_999).await;
    println!("sem saldo: {:?}", tentativa);
    println!("apos falha: Ana={} Bruno={}", saldo(&pool, 1).await?, saldo(&pool, 2).await?);
    assert_eq!(saldo(&pool, 1).await?, 7000);
    assert_eq!(saldo(&pool, 2).await?, 8000);
    Ok(())
}
```

Saida verificada: `antes: Ana=10000 Bruno=5000`, `depois: 7000/8000`, a transferencia sem saldo devolve `Err("saldo insuficiente")` e os saldos **nao mudam** — o ACID do exercicio mora ai: ou os dois UPDATEs ou nenhum. Dinheiro em centavos inteiros, nao `f64`, e o `CHECK (saldo >= 0)` no schema e a segunda linha de defesa.

2. A macro `sqlx::migrate!` le `./migrations` **na compilacao** e embute as SQL no binario; em execucao, `.run(&pool)` aplica as pendentes e registra as aplicadas. Trocar o arquivo depois exige recompilar — migration nao e arquivo de configuracao em tempo de execucao.

Num servidor HTTP, `POST /transferencia` chama esta funcao e traduz `Err(String)` para `400`/`422`; o handler nao conhece SQL — quem conhece transacao e a camada de dominio.

**Autoverificacao:** saldos 7000/8000 apos a transferencia, falha sem saldo sem tocar os dois lados e o banco recriado do zero so com `cargo run` (migrations no inicio).
