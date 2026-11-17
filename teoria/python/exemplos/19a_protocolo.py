"""Aula 19a — framing TCP em Python puro: servidor de linhas com tetos.

Protocolo: linhas UTF-8 terminadas em \\n, teto de 1024 bytes, comandos
PING -> PONG, ECO <txt> -> <txt>, SAIR -> fecha. Qualquer outra coisa,
linha longa ou bytes invalidos: ERRO e a conexao segue viva para os
demais clientes (o erro fecha so a conexao infratora).
"""
import socket
import threading

TETO = 1024
HOST, PORTA = "127.0.0.1", 0  # porta 0: o SO escolhe (teste sem colisao)


def tratar(linha: bytes) -> bytes:
    try:
        texto = linha.decode("utf-8").strip()
    except UnicodeDecodeError:
        return b"ERRO utf8"
    if texto == "PING":
        return b"PONG"
    if texto == "SAIR":
        return b"ATE"
    if texto.startswith("ECO "):
        return texto[4:].encode()
    return b"ERRO comando"


def atende(conn: socket.socket) -> None:
    buf = b""
    with conn:
        while True:
            try:
                dados = conn.recv(4096)
            except OSError:
                return
            if not dados:
                return  # EOF limpo
            buf += dados
            while b"\n" in buf:
                linha, buf = buf.split(b"\n", 1)
                if len(linha) > TETO:
                    conn.sendall(b"ERRO linha_longa\n")
                    return
                resp = tratar(linha)
                conn.sendall(resp + b"\n")
                if resp == b"ATE":
                    return
            if len(buf) > TETO:  # fragmento sem \n alem do teto
                conn.sendall(b"ERRO linha_longa\n")
                return


def subir() -> tuple[socket.socket, int]:
    srv = socket.socket()
    srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    srv.bind((HOST, PORTA))
    srv.listen(16)
    porta = srv.getsockname()[1]

    def loop():
        while True:
            try:
                conn, _ = srv.accept()
            except OSError:
                return
            threading.Thread(target=atende, args=(conn,), daemon=True).start()

    threading.Thread(target=loop, daemon=True).start()
    return srv, porta


def cliente(porta: int) -> socket.socket:
    s = socket.create_connection((HOST, porta), timeout=5)
    return s


def ler_linha(s: socket.socket, buf: bytearray) -> bytes:
    while b"\n" not in buf:
        dados = s.recv(4096)
        if not dados:
            raise ConnectionError("EOF")
        buf += dados
    linha, _, resto = bytes(buf).partition(b"\n")
    buf.clear()
    buf += resto
    return linha


def main() -> int:
    srv, porta = subir()
    buf = bytearray()

    # 1. Fragmentado: PING em dois envios.
    with cliente(porta) as s:
        s.sendall(b"PI")
        s.sendall(b"NG\n")
        assert ler_linha(s, buf) == b"PONG"
    # 2. Coalescido: dois comandos num envio, duas respostas em ordem.
    with cliente(porta) as s:
        buf.clear()
        s.sendall(b"PING\nECO ola\n")
        assert ler_linha(s, buf) == b"PONG"
        assert ler_linha(s, buf) == b"ola"
    # 3. Malformado e utf8 invalido: ERRO, e o servidor segue vivo.
    with cliente(porta) as s:
        buf.clear()
        s.sendall(b"BLAH\n")
        assert ler_linha(s, buf) == b"ERRO comando"
        s.sendall(b"\xff\xfe\n")
        assert ler_linha(s, buf) == b"ERRO utf8"
    # 4. Linha longa: ERRO e so esta conexao fecha.
    with cliente(porta) as s:
        buf.clear()
        s.sendall(b"X" * (TETO + 1) + b"\n")
        assert ler_linha(s, buf) == b"ERRO linha_longa"
    with cliente(porta) as s:
        buf.clear()
        s.sendall(b"PING\n")
        assert ler_linha(s, buf) == b"PONG", "servidor morreu com a linha longa"
    # 5. Cliente lento nao bloqueia os outros (thread por conexao).
    lento = cliente(porta)
    try:
        with cliente(porta) as s:
            buf.clear()
            s.sendall(b"PING\n")
            assert ler_linha(s, buf) == b"PONG"
    finally:
        lento.close()

    srv.close()
    print("19a ok: fragmentado, coalescido, malformado, longo e lento isolados")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
