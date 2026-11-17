from __future__ import annotations

import os
import socket
import subprocess
import sys
import threading
import time
from contextlib import ExitStack
from pathlib import Path
from typing import Final

HOST: Final = "127.0.0.1"


def free_port() -> int:
    with socket.socket() as probe:
        probe.bind((HOST, 0))
        return probe.getsockname()[1]


def connect(port: int) -> socket.socket:
    client = socket.create_connection((HOST, port), timeout=2)
    client.settimeout(2)
    return client


def receive_to_eof(client: socket.socket) -> bytes:
    chunks: list[bytes] = []
    while part := client.recv(4096):
        chunks.append(part)
    return b"".join(chunks)


def receive_exact(client: socket.socket, count: int) -> bytes:
    chunks: list[bytes] = []
    remaining = count
    while remaining:
        part = client.recv(remaining)
        if not part:
            break
        chunks.append(part)
        remaining -= len(part)
    return b"".join(chunks)


def wait_ready(port: int) -> None:
    for _ in range(100):
        try:
            with connect(port):
                return
        except OSError:
            time.sleep(0.02)
    raise AssertionError("servidor nao iniciou")


def check_fragmentation(port: int) -> None:
    with connect(port) as client:
        client.sendall(b"PI")
        client.settimeout(0.1)
        try:
            response = client.recv(1)
        except TimeoutError:
            response = b""
        assert response == b"", response
        client.settimeout(2)
        client.sendall(b"NG\nECHO ola\nQUIT\n")
        assert receive_to_eof(client) == b"PONG\nola\nBYE\n"


def check_invalid_input(port: int) -> None:
    for payload in (b"A" * 1024, b"PING\x00\n"):
        with connect(port) as client:
            client.sendall(payload)
            assert receive_to_eof(client) == b""


def check_disconnect(port: int) -> None:
    with connect(port) as client:
        client.sendall(b"PING\n")
    with connect(port) as client:
        client.sendall(b"PING\n")
        client.shutdown(socket.SHUT_WR)
        assert receive_to_eof(client) == b"PONG\n"


def check_slow_reader(port: int) -> float:
    with connect(port) as slow:
        slow.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 1024)
        slow.settimeout(0.05)
        stop = threading.Event()

        def flood() -> None:
            packet = b"ECHO " + b"x" * 900 + b"\n"
            while not stop.is_set():
                try:
                    slow.sendall(packet)
                except OSError:
                    return

        sender = threading.Thread(target=flood)
        sender.start()
        try:
            time.sleep(0.15)
            start = time.monotonic()
            with connect(port) as fast:
                fast.sendall(b"PING\nQUIT\n")
                assert receive_to_eof(fast) == b"PONG\nBYE\n"
            elapsed = time.monotonic() - start
            assert elapsed < 2, elapsed
            return elapsed
        finally:
            stop.set()
            try:
                slow.shutdown(socket.SHUT_RDWR)
            except OSError:
                pass
            sender.join(timeout=2)
            assert not sender.is_alive()


def check_partial_send(port: int) -> None:
    payload = b"y" * 900
    with connect(port) as client:
        client.sendall(b"ECHO " + payload + b"\nQUIT\n")
        assert receive_to_eof(client) == payload + b"\nBYE\n"


def check_max_clients(port: int) -> None:
    with ExitStack() as stack:
        clients = [stack.enter_context(connect(port)) for _ in range(32)]
        for client in clients:
            client.sendall(b"PING\n")
            assert receive_exact(client, 5) == b"PONG\n"
        with connect(port) as extra:
            assert receive_to_eof(extra) == b""
        clients[0].sendall(b"PING\n")
        assert receive_exact(clients[0], 5) == b"PONG\n"


def main() -> None:
    if len(sys.argv) != 2:
        raise SystemExit("uso: test_aula31.py CAMINHO_DO_BINARIO")
    binary = Path(sys.argv[1]).resolve(strict=True)
    port = free_port()
    env = os.environ.copy()
    preload = env.pop("AULA31_PRELOAD", None)
    if preload is not None:
        assert Path(preload).is_file(), preload
        env["LD_PRELOAD"] = preload
    with subprocess.Popen(
        [str(binary), str(port)],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.PIPE,
        env=env,
    ) as server:
        try:
            wait_ready(port)
            check_fragmentation(port)
            check_invalid_input(port)
            check_disconnect(port)
            elapsed = check_slow_reader(port)
            check_partial_send(port)
            check_max_clients(port)
            assert server.poll() is None
            print(f"PASS: fragmentacao, coalescencia, limites, EOF, leitor lento ({elapsed:.3f}s), envio parcial, 32 clientes")
        finally:
            if server.poll() is None:
                server.terminate()
            try:
                _, stderr = server.communicate(timeout=2)
            except subprocess.TimeoutExpired:
                server.kill()
                _, stderr = server.communicate()
            assert b"ERROR: AddressSanitizer" not in stderr, stderr
            assert b"runtime error:" not in stderr, stderr
            assert b"cannot be preloaded" not in stderr, stderr


if __name__ == "__main__":
    main()
