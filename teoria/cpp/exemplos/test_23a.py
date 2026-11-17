"""Teste do servidor endurecido da aula 23a: sem ele, nada do que a aula
afirma vale. Rode: python3 test_23a.py (compila e sobe o servidor sozinho)."""

from __future__ import annotations

import os
import socket
import subprocess
import sys
import tempfile
import time

HOST = "127.0.0.1"
PORTA = 18091
PORTA_OCIOSA = 18092
DIR = os.path.dirname(os.path.abspath(__file__))
FONTE = os.path.join(DIR, "23a_endurecido.cpp")


class Conexao:
    """Um socket TCP com enquadramento proprio no lado do teste.

    O servidor pode coalescer respostas num unico segmento; sem buffer
    local, uma leitura ingenua devolve duas linhas de uma vez e quebra
    a assercao seguinte. Este e o mesmo framing do servidor, do outro lado.
    """

    def __init__(self, porta: int):
        self.s = socket.create_connection((HOST, porta), timeout=5)
        self.s.settimeout(5)
        self.f = self.s.makefile("rwb")

    def __enter__(self) -> "Conexao":
        return self

    def __exit__(self, *_: object) -> None:
        self.fechar()

    def enviar(self, dados: bytes) -> None:
        self.s.sendall(dados)

    def linha(self) -> bytes:
        l = self.f.readline(65536)
        if not l:
            raise AssertionError("EOF antes da linha")
        return l

    def ler_tudo(self) -> bytes:
        """Le ate EOF (servidor fechou). Falha se algo chegar."""
        saida = b""
        while True:
            parte = self.f.read(4096)
            if not parte:
                return saida
            saida += parte

    def fechado(self, limite: float = 5.0) -> bytes:
        """Consome ate EOF com prazo; o servidor deve fechar sozinho."""
        self.s.settimeout(limite)
        try:
            resto = self.ler_tudo()
        finally:
            self.s.settimeout(5)
        return resto

    def fechar(self) -> None:
        try:
            self.f.close()
        finally:
            self.s.close()


def compilar(destino: str) -> None:
    cmd = ["g++", "-std=c++20", "-Wall", "-Wextra", "-Wpedantic", "-Werror",
           FONTE, "-o", destino]
    r = subprocess.run(cmd, capture_output=True, text=True, timeout=300)
    assert r.returncode == 0, "compilacao:\n" + r.stderr
    print("compilacao ok")


def esperar(porta: int, limite: float = 10.0) -> None:
    t0 = time.time()
    while time.time() - t0 < limite:
        try:
            with socket.create_connection((HOST, porta), timeout=1):
                return
        except OSError:
            time.sleep(0.05)
    raise AssertionError(f"servidor nao abriu na porta {porta}")


def test_basico(porta: int) -> None:
    with conectar(porta) as c:
        c.enviar(b"PING\n")
        assert c.linha() == b"PONG\n"
        c.enviar(b"ECHO ola\n")
        assert c.linha() == b"ola\n"
        c.enviar(b"QUIT\n")
        assert c.linha() == b"BYE\n"
        assert c.fechado() == b"", "esperava EOF apos QUIT drenado"
    print("ok: basico (PING/ECHO/QUIT)")


def conectar(porta: int) -> Conexao:
    return Conexao(porta)


def test_fragmentado(porta: int) -> None:
    with conectar(porta) as c:
        c.enviar(b"PI")
        time.sleep(0.2)
        c.enviar(b"NG\n")
        assert c.linha() == b"PONG\n"
    print("ok: fragmentado (PI + NG)")


def test_coalescido(porta: int) -> None:
    with conectar(porta) as c:
        c.enviar(b"PING\nECHO x\n")
        assert c.linha() == b"PONG\n"
        assert c.linha() == b"x\n"
    print("ok: coalescido (2 msgs num segmento)")


def test_quit_ignora_resto(porta: int) -> None:
    with conectar(porta) as c:
        c.enviar(b"QUIT\nPING\n")
        assert c.linha() == b"BYE\n"
        assert c.fechado() == b"", "nada apos QUIT, nem PONG tardio"
    print("ok: pos-QUIT ignorado (so BYE, depois EOF)")


def test_leitor_lento(porta: int) -> None:
    lenta = conectar(porta)
    try:
        lenta.enviar(b"PING\n")
        # Nao le: fila de saida dela fica cheia de proposito.
        with conectar(porta) as rapida:
            t0 = time.time()
            rapida.enviar(b"PING\n")
            assert rapida.linha() == b"PONG\n"
            dt = time.time() - t0
        assert dt < 3.0, f"cliente rapida travou: {dt:.1f}s"
        assert lenta.linha() == b"PONG\n"
    finally:
        lenta.fechar()
    print("ok: leitor lento nao trava os demais")


def test_linha_grande(porta: int) -> None:
    with conectar(porta) as c:
        c.enviar(b"X" * 9000 + b"\n")
        try:
            resto = c.ler_tudo()
        except ConnectionResetError:
            resto = b"<RST>"
        assert resto in (b"", b"<RST>"), f"esperava EOF/RST sem resposta, veio {resto!r}"
    print("ok: linha de 9000 bytes derrubada sem resposta (EOF ou RST)")


def test_truncado(porta: int) -> None:
    with conectar(porta) as c:
        c.enviar(b"PARCIAL")
        c.s.shutdown(socket.SHUT_WR)  # EOF sem \n: descarta, sem resposta
        assert c.fechado() == b"", "esperava EOF apos truncar"
    with conectar(porta) as c:
        c.enviar(b"PING\n")
        assert c.linha() == b"PONG\n"
    print("ok: truncado descartado, servidor segue vivo")


def test_rajada(porta: int) -> None:
    abertas: list[Conexao] = []
    try:
        for _ in range(40):
            try:
                abertas.append(conectar(porta))
            except OSError:
                break  # recusa no TCP: teto do SO, nao do servidor
        # Solta tudo: o servidor so volta a ter folga quando os
        # descritores fecham do lado dele (EOF no proximo poll).
        for c in abertas:
            c.fechar()
        abertas.clear()
        time.sleep(0.5)
        with conectar(porta) as c:
            c.enviar(b"PING\n")
            assert c.linha() == b"PONG\n", "servidor parou de responder"
    finally:
        for c in abertas:
            c.fechar()
    print("ok: rajada de 40 nao derruba o servico; apos drenar, responde")


def test_ocioso(binario: str) -> None:
    srv = subprocess.Popen([binario, str(PORTA_OCIOSA), "2"],
                           stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    try:
        esperar(PORTA_OCIOSA)
        with conectar(PORTA_OCIOSA) as c:
            time.sleep(4.0)  # excede o ocioso de 2 s
            assert c.fechado() == b"", "esperava EOF por ociosidade"
    finally:
        srv.terminate()
        srv.wait(timeout=10)
    print("ok: ocioso alem de 2s e desconectado")


def main() -> int:
    tmp = tempfile.mkdtemp(prefix="aula23a_")
    binario = os.path.join(tmp, "srv23a")
    compilar(binario)
    srv = subprocess.Popen([binario, str(PORTA), "10"],
                           stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    try:
        esperar(PORTA)
        test_basico(PORTA)
        test_fragmentado(PORTA)
        test_coalescido(PORTA)
        test_quit_ignora_resto(PORTA)
        test_leitor_lento(PORTA)
        test_linha_grande(PORTA)
        test_truncado(PORTA)
        test_rajada(PORTA)
    finally:
        srv.terminate()
        srv.wait(timeout=10)
    test_ocioso(binario)
    print("AULA23A_OK: framing, EAGAIN, timeout, backpressure e rajada")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
