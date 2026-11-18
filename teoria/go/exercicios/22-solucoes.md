---
layout: default
---

# Aula 22 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../22.md#21-exercicios-obrigatorios). Executado com `database/sql` + driver SQLite (`github.com/mattn/go-sqlite3`, cgo) em banco temporario:

```go
package main

import (
	"context"
	"database/sql"
	"errors"
	"fmt"
	"os"
	"path/filepath"
	"time"

	_ "github.com/mattn/go-sqlite3"
)

type usuario struct {
	ID    int64
	Nome  string
	Email string
}

var errNaoEncontrado = errors.New("usuario nao encontrado")

func buscarPorEmail(ctx context.Context, db *sql.DB, email string) (usuario, error) {
	var u usuario
	err := db.QueryRowContext(ctx,
		"SELECT id, nome, email FROM usuarios WHERE email = ?", email).
		Scan(&u.ID, &u.Nome, &u.Email)
	if errors.Is(err, sql.ErrNoRows) {
		return usuario{}, errNaoEncontrado
	}
	if err != nil {
		return usuario{}, fmt.Errorf("buscar por email: %w", err)
	}
	return u, nil
}

func listar(ctx context.Context, db *sql.DB) ([]usuario, error) {
	rows, err := db.QueryContext(ctx, "SELECT id, nome, email FROM usuarios ORDER BY id")
	if err != nil {
		return nil, fmt.Errorf("listar: %w", err)
	}
	defer rows.Close()
	var lista []usuario
	for rows.Next() {
		var u usuario
		if err := rows.Scan(&u.ID, &u.Nome, &u.Email); err != nil {
			return nil, fmt.Errorf("scan: %w", err)
		}
		lista = append(lista, u)
	}
	return lista, rows.Err()
}

func criarComBoasVindas(ctx context.Context, db *sql.DB, u usuario) error {
	tx, err := db.BeginTx(ctx, nil)
	if err != nil {
		return err
	}
	defer tx.Rollback()
	if _, err := tx.ExecContext(ctx,
		"INSERT INTO usuarios(nome, email) VALUES(?, ?)", u.Nome, u.Email); err != nil {
		return fmt.Errorf("inserir usuario: %w", err)
	}
	if _, err := tx.ExecContext(ctx,
		"INSERT INTO mensagens(usuario_email, texto) VALUES(?, ?)", u.Email, "bem-vindo"); err != nil {
		return fmt.Errorf("inserir mensagem: %w", err)
	}
	return tx.Commit()
}

func main() {
	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()
	caminho := filepath.Join(os.TempDir(), "aula22_usuarios.db")
	_ = os.Remove(caminho)
	defer os.Remove(caminho)
	db, err := sql.Open("sqlite3", caminho)
	if err != nil {
		panic(err)
	}
	defer db.Close()
	// SQLite grava por um escritor; 1 conexao evita "database is locked" neste laboratorio.
	db.SetMaxOpenConns(1)
	db.SetMaxIdleConns(1)
	db.SetConnMaxLifetime(time.Minute)

	if _, err := db.ExecContext(ctx, `CREATE TABLE usuarios(
		id INTEGER PRIMARY KEY, nome TEXT NOT NULL, email TEXT NOT NULL UNIQUE)`,
	); err != nil {
		panic(err)
	}
	if _, err := db.ExecContext(ctx, `CREATE TABLE mensagens(
		id INTEGER PRIMARY KEY, usuario_email TEXT NOT NULL, texto TEXT NOT NULL)`,
	); err != nil {
		panic(err)
	}

	if err := criarComBoasVindas(ctx, db, usuario{Nome: "Ada", Email: "ada@exemplo.dev"}); err != nil {
		panic(err)
	}
	if err := criarComBoasVindas(ctx, db, usuario{Nome: "Beto", Email: "beto@exemplo.dev"}); err != nil {
		panic(err)
	}

	u, err := buscarPorEmail(ctx, db, "ada@exemplo.dev")
	fmt.Println("busca:", u.Nome, u.Email, "| erro:", err)
	_, err = buscarPorEmail(ctx, db, "ninguem@exemplo.dev")
	fmt.Println("ausente mapeado:", errors.Is(err, errNaoEncontrado))

	lista, err := listar(ctx, db)
	if err != nil {
		panic(err)
	}
	fmt.Println("listados:", len(lista), lista[0].Nome, lista[1].Nome)

	// Transacao que falha no segundo passo nao deixa o primeiro para tras.
	duplicado := criarComBoasVindas(ctx, db, usuario{Nome: "Clone", Email: "ada@exemplo.dev"})
	listaDepois, _ := listar(ctx, db)
	fmt.Println("tx duplicada falhou:", duplicado != nil, "| total apos rollback:", len(listaDepois))
	fmt.Println("testes ok")
}
```

5. Pool: `MaxOpenConns=1` aqui nao e receita geral — e a restricao real do SQLite (um escritor por vez) transformada em configuracao. Contra Postgres, o teto sai do limite do banco dividido pelas replicas do servico; `ConnMaxLifetime` recicla conexoes antes que firewalls e o banco as matem por idade. Pool sem teto nao e generosidade: e fila escondida dentro do banco.

6. N+1 deste proprio exemplo: listar usuarios e, para cada um, consultar suas mensagens em outro `QueryRow`. Com 2 usuarios sao 3 consultas; com 10 mil, 10001. Corrige-se com **um** `JOIN` (traz tudo junto) ou um `IN` com os emails da pagina — a contagem de consultas por requisicao e a metrica que denuncia.

**Autoverificacao:** busca encontra, ausente vira `errNaoEncontrado` via `errors.Is`, listagem ordenada e a transacao duplicada falha sem deixar usuario orfao.
