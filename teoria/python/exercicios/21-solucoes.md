---
layout: default
---

# Aula 21 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../21.md#13-exercicios-obrigatorios). Executado com `sqlite3` da biblioteca padrao:

```python
import sqlite3
import tempfile
from pathlib import Path


class EmailDuplicado(Exception):
    pass


def criar_tabelas(con: sqlite3.Connection) -> None:
    con.execute("CREATE TABLE usuarios(id INTEGER PRIMARY KEY, nome TEXT, email TEXT UNIQUE)")
    con.execute("CREATE TABLE boas_vindas(id INTEGER PRIMARY KEY, usuario_id INTEGER, texto TEXT)")


def criar_usuario(con: sqlite3.Connection, nome: str, email: str) -> int:
    try:
        with con:  # transacao: as duas escritas ou nenhuma (exercicio 2)
            cur = con.execute("INSERT INTO usuarios(nome, email) VALUES(?, ?)", (nome, email))
            con.execute("INSERT INTO boas_vindas(usuario_id, texto) VALUES(?, ?)",
                        (cur.lastrowid, "bem-vindo"))
        return int(cur.lastrowid)
    except sqlite3.IntegrityError as e:
        raise EmailDuplicado(email) from e


def main() -> None:
    with tempfile.TemporaryDirectory() as tmp:
        con = sqlite3.connect(Path(tmp) / "aula21.db")
        criar_tabelas(con)
        ada = criar_usuario(con, "Ada", "ada@exemplo.dev")
        criar_usuario(con, "Beto", "beto@exemplo.dev")
        try:
            criar_usuario(con, "Clone", "ada@exemplo.dev")
        except EmailDuplicado:
            print("duplicado barrado no UNIQUE e traduzido no servico")

        # N+1 contado: 1 consulta pelos usuarios + 1 por usuario.
        consultas = 0
        for (uid,) in con.execute("SELECT id FROM usuarios").fetchall():
            consultas += 1
            con.execute("SELECT texto FROM boas_vindas WHERE usuario_id = ?", (uid,)).fetchall()
        consultas += 1
        com_join = con.execute(
            "SELECT u.nome, b.texto FROM usuarios u "
            "JOIN boas_vindas b ON b.usuario_id = u.id ORDER BY u.id").fetchall()
        print(f"N+1 executou {consultas} consultas; JOIN executou 1 -> {com_join}")
        assert consultas == 3 and len(com_join) == 2 and com_join[0][0] == "Ada"
        con.close()
    print("testes ok")


if __name__ == "__main__":
    main()
```

3. Pool de conexoes resolve o custo e a disputa de **abrir** conexoes: sem pool, cada requisicao paga handshake e autenticacao, e mil requisicoes simultaneas tentam abrir mil conexoes contra um banco que aceita cem. Pool nao acelera consulta lenta nem substitui indice — ele administra um recurso caro e limitado, como o pool da [aula 22 de Go](../../go/exercicios/22-solucoes.md).

5. O ORM esconde a sintaxe do SQL, nao a existencia dele: sem indice em `email`, o `UNIQUE` e o `WHERE` varrem a tabela; com latencia de rede por consulta, o N+1 acima custa 3 viagens onde o JOIN custa 1. O ORM nao ve seu plano de execucao nem seu orcamento de latencia — voce continua devendo os dois.

**Autoverificacao:** duplicado traduzido, transacao com duas escritas e N+1 contado contra o JOIN de uma consulta.
