"""Dois nos locais com log duravel, idempotencia e recuperacao.

Uso:
    python3 23a_distribuido.py no --porta 9001 --par 127.0.0.1:9002 --log no1.log
    python3 23a_distribuido.py checar   # roteiro completo de falhas
"""

import json
import os
import signal
import socket
import subprocess
import sys
import tempfile
import threading
import time


class No:
    def __init__(self, porta, par, caminho_log, atraso_ms=0.0):
        self.porta = porta
        self.par = par  # "host:porta" ou ""
        self.caminho_log = caminho_log
        self.atraso = atraso_ms / 1000.0
        self.estado = {}
        self.vistos = {}  # id -> resultado (dedup sobrevive via replay do log)
        self.lock = threading.Lock()  # cheque+persiste+aplica sao atomicos juntos

    def carregar(self):
        if not os.path.exists(self.caminho_log):
            return
        with open(self.caminho_log, "r", encoding="utf-8") as f:
            for linha in f:
                linha = linha.strip()
                if not linha:
                    continue
                try:
                    reg = json.loads(linha)
                except ValueError:
                    continue
                if reg.get("op") == "set":
                    self.estado[reg["chave"]] = reg["valor"]
                    self.vistos[reg["id"]] = {"aplicado": True}

    def persistir(self, reg):
        with open(self.caminho_log, "a", encoding="utf-8") as f:
            f.write(json.dumps(reg, sort_keys=True) + "\n")
            f.flush()
            os.fsync(f.fileno())

    def encaminhar(self, pedido):
        if not self.par:
            return
        if self.atraso:
            time.sleep(self.atraso)
        host, p = self.par.rsplit(":", 1)
        try:
            with socket.create_connection((host, int(p)), timeout=2) as s:
                s.settimeout(2)
                f = s.makefile("rwb")
                f.write((json.dumps(pedido) + "\n").encode())
                f.flush()
                f.readline(65536)
        except OSError:
            pass  # par fora do ar: degrada, nao trava

    def tratar(self, pedido):
        pid = pedido.get("id")
        op = pedido.get("op")
        if not pid or op not in ("set", "get"):
            return {"id": pid, "ok": False, "erro": "pedido invalido"}
        with self.lock:
            if pid in self.vistos:
                return {"id": pid, "ok": True, "resultado": self.vistos[pid], "repetido": True}
            if op == "get":
                return {"id": pid, "ok": True, "resultado": self.estado.get(pedido.get("chave"))}
            chave, valor = pedido.get("chave"), pedido.get("valor")
            if not isinstance(chave, str):
                return {"id": pid, "ok": False, "erro": "chave invalida"}
            self.persistir({"id": pid, "op": "set", "chave": chave, "valor": valor})
            self.estado[chave] = valor
            resultado = {"aplicado": True}
            self.vistos[pid] = resultado
        # Encaminhar FORA do lock: I/O de rede sob lock serializaria tudo.
        self.encaminhar(pedido)
        return {"id": pid, "ok": True, "resultado": resultado, "repetido": False}

    def atender(self, conn):
        try:
            with conn:
                f = conn.makefile("rwb")
                linha = f.readline(1_000_000 + 2)
                if not linha:
                    return
                try:
                    pedido = json.loads(linha.decode())
                except ValueError:
                    f.write(b'{"ok": false, "erro": "json invalido"}\n')
                    f.flush()
                    return
                if not isinstance(pedido, dict):
                    f.write(b'{"ok": false, "erro": "pedido invalido"}\n')
                    f.flush()
                    return
                resp = self.tratar(pedido)
                f.write((json.dumps(resp) + "\n").encode())
                f.flush()
        except (OSError, ValueError):
            return

    def servir(self):
        srv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        srv.bind(("127.0.0.1", self.porta))
        srv.listen(32)
        # Uma thread por conexao: aceitar-e-responder na mesma thread
        # travaria o encaminhamento mutuo (A espera B, B espera A) e so
        # o timeout desfaria o no. Concorrencia aqui e correcao, nao luxo.
        while True:
            conn, _ = srv.accept()
            t = threading.Thread(target=self.atender, args=(conn,), daemon=True)
            t.start()


def executar_no(args):
    import argparse
    ap = argparse.ArgumentParser()
    ap.add_argument("--porta", type=int, required=True)
    ap.add_argument("--par", default="")
    ap.add_argument("--log", required=True)
    ap.add_argument("--atraso-ms", type=float, default=0.0)
    ns = ap.parse_args(args)
    no = No(ns.porta, ns.par, ns.log, ns.atraso_ms)
    no.carregar()
    no.servir()


# ------------------------------ roteiro ------------------------------

def chamar(porta, pedido, timeout=5):
    with socket.create_connection(("127.0.0.1", porta), timeout=timeout) as s:
        s.settimeout(timeout)
        f = s.makefile("rwb")
        f.write((json.dumps(pedido) + "\n").encode())
        f.flush()
        linha = f.readline(1_000_000)
        assert linha, "sem resposta"
        return json.loads(linha.decode())


def esperar_porta(porta, limite=10.0):
    t0 = time.time()
    while time.time() - t0 < limite:
        try:
            with socket.create_connection(("127.0.0.1", porta), timeout=0.5):
                return
        except OSError:
            time.sleep(0.1)
    raise AssertionError(f"porta {porta} nao abriu")


def porta_livre():
    with socket.socket() as s:
        s.bind(("127.0.0.1", 0))
        return s.getsockname()[1]


def checar():
    tmp = tempfile.mkdtemp(prefix="aula23a_")
    log_a = os.path.join(tmp, "a.log")
    log_b = os.path.join(tmp, "b.log")
    pa, pb = porta_livre(), porta_livre()
    eu = [sys.executable, os.path.abspath(__file__)]

    a = subprocess.Popen(eu + ["no", "--porta", str(pa), "--par", f"127.0.0.1:{pb}", "--log", log_a])
    b = subprocess.Popen(eu + ["no", "--porta", str(pb), "--par", f"127.0.0.1:{pa}", "--log", log_b])
    try:
        esperar_porta(pa)
        esperar_porta(pb)

        # 1-2. Escrita via A replica em B; leitura nos dois.
        r = chamar(pa, {"id": "op1", "op": "set", "chave": "k1", "valor": 10})
        assert r["ok"] and not r.get("repetido"), r
        assert chamar(pa, {"id": "g1", "op": "get", "chave": "k1"})["resultado"] == 10
        assert chamar(pb, {"id": "g2", "op": "get", "chave": "k1"})["resultado"] == 10
        print("replicacao ok: k1=10 nos dois nos")

        # 3. Duplicata com valor diferente: vale a primeira.
        r = chamar(pa, {"id": "op1", "op": "set", "chave": "k1", "valor": 999})
        assert r["ok"] and r.get("repetido"), r
        assert chamar(pa, {"id": "g3", "op": "get", "chave": "k1"})["resultado"] == 10
        print("idempotencia ok: duplicata nao reaplicou")

        # 4-5. kill -9 em B; escrita via A degrada mas responde.
        b.send_signal(signal.SIGKILL)
        assert b.wait(timeout=10) is not None
        r = chamar(pa, {"id": "op2", "op": "set", "chave": "k2", "valor": 20})
        assert r["ok"] and not r.get("repetido"), r
        assert chamar(pa, {"id": "g4", "op": "get", "chave": "k2"})["resultado"] == 20
        print("degradacao ok: A responde com B morto")

        # Log de B sobreviveu ao kill (fsync por registro).
        with open(log_b, encoding="utf-8") as f:
            ids_b = [json.loads(l)["id"] for l in f if l.strip()]
        assert "op1" in ids_b, ids_b

        # 6. Reinicia B com o MESMO log: replay, sem k2 ainda.
        b = subprocess.Popen(eu + ["no", "--porta", str(pb), "--par", f"127.0.0.1:{pa}", "--log", log_b])
        esperar_porta(pb)
        assert chamar(pb, {"id": "g5", "op": "get", "chave": "k1"})["resultado"] == 10
        assert chamar(pb, {"id": "g6", "op": "get", "chave": "k2"})["resultado"] is None
        print("recuperacao ok: replay trouxe k1, k2 ainda ausente")

        # 7. Reenvio com o MESMO id converge sem duplicar.
        r = chamar(pb, {"id": "op2", "op": "set", "chave": "k2", "valor": 20})
        assert r["ok"], r
        assert chamar(pa, {"id": "g7", "op": "get", "chave": "k2"})["resultado"] == 20
        assert chamar(pb, {"id": "g8", "op": "get", "chave": "k2"})["resultado"] == 20
        print("convergencia ok: reenvio idempotente, k2=20 nos dois")

        # 8. Atraso no encaminhamento nao trava o cliente (timeout 2s > 0.3s).
        a.terminate()
        a.wait(timeout=10)
        a = subprocess.Popen(eu + ["no", "--porta", str(pa), "--par", f"127.0.0.1:{pb}",
                                   "--log", log_a, "--atraso-ms", "300"])
        esperar_porta(pa)
        t0 = time.time()
        r = chamar(pa, {"id": "op3", "op": "set", "chave": "k3", "valor": 30}, timeout=10)
        dt = time.time() - t0
        assert r["ok"], r
        assert dt < 5, dt
        print(f"atraso ok: resposta em {dt:.2f}s apesar do par lento")

        # Estado final igual nos dois nos.
        for k, v in (("k1", 10), ("k2", 20), ("k3", 30)):
            ga = chamar(pa, {"id": f"fa-{k}", "op": "get", "chave": k})["resultado"]
            gb = chamar(pb, {"id": f"fb-{k}", "op": "get", "chave": k})["resultado"]
            assert ga == gb == v, (k, ga, gb)
        print("estado final igual: k1=10 k2=20 k3=30")
        print("distribuido ok: repeticao, queda, reinicio, atraso e recuperacao")
        return 0
    finally:
        for p in (a, b):
            try:
                if p.poll() is None:
                    p.terminate()
                    p.wait(timeout=5)
            except Exception:
                try:
                    p.kill()
                except Exception:
                    pass


if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "no":
        executar_no(sys.argv[2:])
    elif len(sys.argv) > 1 and sys.argv[1] == "checar":
        raise SystemExit(checar())
    else:
        print("uso: 23a_distribuido.py (no ... | checar)", file=sys.stderr)
        raise SystemExit(2)
