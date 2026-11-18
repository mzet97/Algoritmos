#!/usr/bin/env python3
"""Teste dos tetos globais do servidor 23f (C): por-IP, global e bytes.

Uso: python3 test_23f.py BINARIO
Sobe o servidor em porta efemera e prova: 3a conexao do mesmo IP cai
com CHEIO por-ip; a 9a conexao simultanea (2 por IP em 127.0.0.1-4) cai
com CHEIO global; rajada acima de 64 KiB na janela silencia com aviso e
recupera; e cliente rapido e atendido enquanto lentos ocupam os demais slots.
"""
from __future__ import annotations

import socket
import subprocess
import sys
import time
from pathlib import Path
from typing import Final

HOST: Final = "127.0.0.1"


def free_port() -> int:
    with socket.socket() as probe:
        probe.bind((HOST, 0))
        return probe.getsockname()[1]


def conecta(origem: str, porta: int) -> socket.socket:
    s = socket.socket()
    s.settimeout(10)
    s.bind((origem, 0))
    s.connect((HOST, porta))
    return s


def linha(s: socket.socket) -> bytes:
    buf = b""
    while not buf.endswith(b"\n"):
        parte = s.recv(256)
        if not parte:
            break
        buf += parte
    return buf


def main() -> None:
    if len(sys.argv) != 2:
        print("uso: test_23f.py BINARIO", file=sys.stderr)
        raise SystemExit(2)
    binario = Path(sys.argv[1]).resolve(strict=True)
    porta = free_port()
    with subprocess.Popen([str(binario), str(porta)], stderr=subprocess.PIPE) as srv:
        try:
            for _ in range(100):
                try:
                    with conecta(HOST, porta):
                        break
                except OSError:
                    time.sleep(0.05)
            else:
                raise AssertionError("servidor nao iniciou")

            # Teto por IP: 2 passam calados, a 3a cai com motivo.
            lentos = [conecta("127.0.0.1", porta), conecta("127.0.0.1", porta)]
            time.sleep(0.5)
            with conecta("127.0.0.1", porta) as extra:
                assert linha(extra) == b"CHEIO por-ip\n"
            print("por-ip ok: 3a conexao do mesmo IP recusada")

            # Teto global: 2 por IP em .1-.4 = 8; a 9a cai.
            for ip in ("127.0.0.2", "127.0.0.3", "127.0.0.4"):
                lentos.append(conecta(ip, porta))
                lentos.append(conecta(ip, porta))
            time.sleep(0.5)
            with conecta("127.0.0.5", porta) as alem:
                assert linha(alem) == b"CHEIO global\n"
            print("global ok: 9a conexao simultanea recusada")

            # Lento nao paralisa: libera 1 slot e o rapido e atendido.
            lentos.pop(0).close()
            time.sleep(0.5)
            with conecta("127.0.0.1", porta) as rapido:
                rapido.sendall(b"ECO oi\n")
                assert linha(rapido) == b"ECO ECO oi\n"
            print("lentos ok: rapido atendido com 7 lentos ocupando slots")

            # Teto de linha: 2000 bytes sem \n fecham so esta conexao.
            with conecta("127.0.0.1", porta) as longa:
                longa.sendall(b"x" * 2000)
                assert linha(longa) == b"ERRO linha-longa\n"
            print("linha ok: linha longa e erro da conexao, servidor vivo")

            # Teto de bytes: 210 KiB de linhas curtas silenciam a conexao
            # ate a proxima janela (fechar aqui geraria RST com a rajada
            # nao lida e o cliente nunca leria o motivo).
            with conecta("127.0.0.1", porta) as rajada:
                rajada.sendall(b"ECO x\n" * 30000)
                achou = False
                resto = b""
                while not achou:
                    parte = rajada.recv(4096)
                    assert parte, "conexao caiu sem avisar"
                    resto += parte
                    while b"\n" in resto:
                        uma, resto = resto.split(b"\n", 1)
                        if uma == b"CHEIO bytes":
                            achou = True
                            break
                assert achou, "CHEIO bytes nao chegou"
                # Esvazia ecos velhos da rajada e espera a janela virar:
                # o reset e preguicoso (so acontece quando ha trafego).
                rajada.settimeout(0.3)
                try:
                    while rajada.recv(4096):
                        pass
                except socket.timeout:
                    pass
                rajada.settimeout(10)
                time.sleep(1.2)
                rajada.sendall(b"ECO de-novo\n")
                assert linha(rajada) == b"ECO ECO de-novo\n"
            print("bytes ok: rajada silenciada com aviso e recuperada na janela seguinte")

            # Servidor segue vivo depois de todas as recusas.
            with conecta("127.0.0.1", porta) as vivo:
                vivo.sendall(b"ECO fim\n")
                assert linha(vivo) == b"ECO ECO fim\n"
            print("23f ok: tetos por-ip, global e de bytes; lentos nao paralisam")
        finally:
            if srv.poll() is None:
                srv.terminate()
            try:
                _, stderr = srv.communicate(timeout=2)
            except subprocess.TimeoutExpired:
                srv.kill()
                _, stderr = srv.communicate()
            assert b"ERROR: AddressSanitizer" not in stderr, stderr
            assert b"runtime error:" not in stderr, stderr


if __name__ == "__main__":
    main()
