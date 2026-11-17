#!/usr/bin/env python3
"""Executa os projetos das aulas Rust 30-32 e Python 30-32 em ambiente limpo.

Fontes: teoria/rust/projetos/* (Cargo, extraidas das aulas 30, 31 e 32 bonus)
e teoria/python/projetos/* (ambiente .venv gerenciado por uv no proprio
projeto). Uso: python3 teoria/verificar_projetos.py
Sai 0 somente se todas as verificacoes passarem.
"""
import hashlib
import socket
import subprocess
import sys
import tempfile
import threading
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
RUST = ROOT / "teoria/rust/projetos"
PYPROJ = ROOT / "teoria/python/projetos"
FALHAS = []


def check(nome, cond, detalhe=""):
    if cond:
        print(f"ok - {nome}")
    else:
        FALHAS.append(nome)
        print(f"FALHA - {nome} {detalhe}")


def encerrar(proc):
    proc.terminate()
    try:
        proc.wait(timeout=5)
    except subprocess.TimeoutExpired:
        proc.kill()
        proc.wait(timeout=5)


def espera_porta_livre(port, timeout=15.0):
    fim = time.time() + timeout
    while time.time() < fim:
        try:
            with socket.create_connection(("127.0.0.1", port), timeout=0.3):
                time.sleep(0.1)
        except OSError:
            return True
    return False


def wait_port(port, proc, timeout=15.0):
    fim = time.time() + timeout
    while time.time() < fim:
        if proc.poll() is not None:
            return False
        try:
            with socket.create_connection(("127.0.0.1", port), timeout=0.5):
                return True
        except OSError:
            time.sleep(0.1)
    return False


class Linha:
    """Cliente TCP de linhas CRLF para os mini-Redis."""

    def __init__(self, port):
        self.sock = socket.create_connection(("127.0.0.1", port), timeout=10)
        self.buf = b""

    def cmd(self, texto):
        self.sock.sendall(texto.encode() + b"\r\n")
        while b"\r\n" not in self.buf:
            dados = self.sock.recv(4096)
            if not dados:
                raise ConnectionError("servidor fechou a conexao")
            self.buf += dados
        linha, self.buf = self.buf.split(b"\r\n", 1)
        return linha.decode()

    def close(self):
        self.sock.close()


def backend_http(port, tag, parar):
    srv = socket.socket()
    srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    srv.bind(("127.0.0.1", port))
    srv.listen(16)
    srv.settimeout(0.5)
    while not parar.is_set():
        try:
            conn, _ = srv.accept()
        except TimeoutError:
            continue
        with conn:
            conn.settimeout(5)
            try:
                dados = b""
                while b"\r\n\r\n" not in dados:
                    bloco = conn.recv(4096)
                    if not bloco:
                        break
                    dados += bloco
                corpo = tag.encode()
                conn.sendall(
                    b"HTTP/1.1 200 OK\r\nContent-Length: "
                    + str(len(corpo)).encode()
                    + b"\r\nConnection: close\r\n\r\n"
                    + corpo
                )
            except OSError:
                pass
    srv.close()


def testar_load_balancer():
    parar = threading.Event()
    for porta in (8081, 8082, 8083):
        threading.Thread(
            target=backend_http, args=(porta, f"BACKEND-{porta}", parar), daemon=True
        ).start()
    proc = subprocess.Popen(
        [str(RUST / "30_load_balancer/target/release/load_balancer_30")],
        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    try:
        check("rust30/porta-8080", wait_port(8080, proc))
        ordem = []
        for _ in range(6):
            with socket.create_connection(("127.0.0.1", 8080), timeout=5) as s:
                s.sendall(b"GET / HTTP/1.1\r\nHost: x\r\nConnection: close\r\n\r\n")
                s.shutdown(socket.SHUT_WR)
                resp = b""
                while True:
                    bloco = s.recv(4096)
                    if not bloco:
                        break
                    resp += bloco
            ordem.append(resp.rsplit(b"\r\n\r\n", 1)[-1].decode())
        ciclo = [f"BACKEND-{p}" for p in (8081, 8082, 8083)]
        # A conexao de sondagem (wait_port) tambem consome um indice do
        # round-robin; o que se exige e a sequencia ciclica, nao a fase.
        ciclico = len(ordem) == 6 and all(
            ordem[i] == ciclo[(ciclo.index(ordem[0]) + i) % 3] for i in range(6)
        ) if ordem and ordem[0] in ciclo else False
        check("rust30/round-robin", ciclico, str(ordem))
    finally:
        encerrar(proc)
        parar.set()


def testar_mini_redis(binario, prefixo, avancado):
    check(f"{prefixo}/porta-6379-livre-antes", espera_porta_livre(6379))
    proc = subprocess.Popen(
        [str(binario)], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    try:
        check(f"{prefixo}/porta-6379", wait_port(6379, proc))
        c = Linha(6379)
        check(f"{prefixo}/ping", c.cmd("PING") == "+PONG")
        check(f"{prefixo}/set", c.cmd("SET nome Engenheiro") == "+OK")
        check(f"{prefixo}/get", c.cmd("GET nome") == "+Engenheiro")
        check(f"{prefixo}/get-ausente", c.cmd("GET idade") == "$-1")
        check(f"{prefixo}/del", c.cmd("DEL nome") == ":1")
        check(f"{prefixo}/del-ausente", c.cmd("DEL nome") == ":0")
        check(f"{prefixo}/desconhecido",
              c.cmd("FOO x").startswith("-ERR Comando desconhecido"))
        if avancado:
            check("rust32/set-contador", c.cmd("SET contador 0") == "+OK")
            erros = []

            def martelar():
                try:
                    cli = Linha(6379)
                    for _ in range(250):
                        r = cli.cmd("INCR contador")
                        if not r.startswith(":"):
                            erros.append(r)
                    cli.close()
                except Exception as exc:  # noqa: BLE001 - vira falha nomeada
                    erros.append(repr(exc))

            threads = [threading.Thread(target=martelar) for _ in range(2)]
            for t in threads:
                t.start()
            for t in threads:
                t.join()
            check("rust32/incr-concorrente-500",
                  not erros and c.cmd("GET contador") == "+500",
                  f"erros={erros[:3]}")
            check("rust32/set-texto", c.cmd("SET texto abc") == "+OK")
            check("rust32/incr-texto-erro",
                  c.cmd("INCR texto").startswith("-ERR"))
            check("rust32/set-max", c.cmd(f"SET maximo {2**63 - 1}") == "+OK")
            check("rust32/incr-overflow-erro",
                  c.cmd("INCR maximo").startswith("-ERR"))
            check("rust32/ttl-set", c.cmd("SET sessao abc EX1") == "+OK")
            check("rust32/ttl-antes", c.cmd("GET sessao") == "+abc")
            time.sleep(1.4)
            check("rust32/ttl-depois", c.cmd("GET sessao") == "$-1")
        else:
            def cliente(tag):
                cli = Linha(6379)
                assert cli.cmd(f"SET chave-{tag} valor-{tag}") == "+OK"
                assert cli.cmd(f"GET chave-{tag}") == f"+valor-{tag}"
                cli.close()

            threads = [threading.Thread(target=cliente, args=(i,)) for i in range(8)]
            for t in threads:
                t.start()
            for t in threads:
                t.join()
            check("rust31/clientes-concorrentes", True)
        c.close()
    finally:
        encerrar(proc)


def testar_load_balancer_isolado():
    """A aula fixa a porta 8080; se ela estiver ocupada na maquina,
    o teste roda em namespace de rede proprio (unshare -rn)."""
    res = rodar(["unshare", "-rn", sys.executable, str(Path(__file__).resolve()),
                 "--so-lb"], timeout=180)
    print(res.stdout, end="")
    if res.returncode == 0 and "ok - rust30/round-robin" in res.stdout:
        return
    if "unshare" in res.stderr or "Operation not permitted" in res.stderr:
        testar_load_balancer()
        return
    if "FALHA" not in res.stdout:
        check("rust30/round-robin", False, res.stderr[-300:])


def rodar(cmd, cwd=None, timeout=300):
    return subprocess.run(cmd, cwd=cwd, capture_output=True, text=True,
                          timeout=timeout)


def testar_python():
    py = PYPROJ / ".venv/bin/python"
    if not py.exists():
        uv = rodar(["uv", "sync", "--project", str(PYPROJ)], timeout=600)
        check("python/uv-sync", uv.returncode == 0, uv.stderr[-400:])
    check("python/venv-no-projeto", py.exists())
    if not py.exists():
        return
    pytest = rodar([str(py), "-m", "pytest", "test_api.py", "-q"],
                   cwd=PYPROJ / "30_backend")
    check("python30/pytest", pytest.returncode == 0,
          (pytest.stdout + pytest.stderr)[-400:])

    servidor = subprocess.Popen(
        [str(py), "-m", "uvicorn", "main:app", "--host", "127.0.0.1",
         "--port", "8765"],
        cwd=PYPROJ / "30_backend",
        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    try:
        check("python30/uvicorn-sobe", wait_port(8765, servidor))
        import urllib.request
        with urllib.request.urlopen("http://127.0.0.1:8765/health",
                                    timeout=5) as r:
            check("python30/health-ao-vivo", r.status == 200)
    finally:
        encerrar(servidor)

    with tempfile.TemporaryDirectory() as tmp:
        Path(tmp, "a.txt").write_text("a\n")
        Path(tmp, "b.txt").write_text("b\n")
        r1 = rodar([str(py), str(PYPROJ / "31_infra_tool/main.py"),
                    "a.txt", "b.txt"], cwd=tmp)
        esperado = hashlib.sha256(b"a\n").hexdigest()
        check("python31/primeira-execucao",
              r1.returncode == 0 and f"sha256={esperado}" in r1.stdout,
              r1.stdout + r1.stderr)
        r2 = rodar([str(py), str(PYPROJ / "31_infra_tool/main.py"),
                    "a.txt", "b.txt"], cwd=tmp)
        check("python31/reexecucao-pula",
              r2.returncode == 0 and r2.stdout.count("[skip]") == 2, r2.stdout)
        r3 = rodar([str(py), str(PYPROJ / "31_infra_tool/main.py"),
                    "ausente.txt"], cwd=tmp)
        check("python31/ausente-exit-2", r3.returncode == 2,
              f"rc={r3.returncode}")

    r32 = rodar([str(py), str(PYPROJ / "32_pipeline/main.py")])
    check("python32/pipeline",
          r32.returncode == 0 and "blue" in r32.stdout
          and "65.0" in r32.stdout and "elapsed_ms=" in r32.stdout,
          (r32.stdout + r32.stderr)[-400:])


def main():
    if "--so-lb" in sys.argv:
        rodar(["ip", "link", "set", "lo", "up"])
        testar_load_balancer()
        return 1 if FALHAS else 0
    for projeto in ("30_load_balancer", "31_mini_redis",
                    "32_mini_redis_avancado"):
        build = rodar(["cargo", "build", "--release", "--manifest-path",
                       str(RUST / projeto / "Cargo.toml")], timeout=600)
        check(f"rust/{projeto}-compila", build.returncode == 0,
              build.stderr[-400:])
    if FALHAS:
        print(f"projetos: {len(FALHAS)} falhas iniciais: {FALHAS}")
        return 1
    testar_load_balancer_isolado()
    testar_mini_redis(RUST / "31_mini_redis/target/release/mini_redis_31",
                      "rust31", avancado=False)
    testar_mini_redis(
        RUST / "32_mini_redis_avancado/target/release/mini_redis_avancado_32",
        "rust32", avancado=True)
    testar_python()
    print(f"projetos: {'FALHOU ' + str(FALHAS) if FALHAS else 'tudo verde'}")
    return 1 if FALHAS else 0


if __name__ == "__main__":
    sys.exit(main())
