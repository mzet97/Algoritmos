#!/usr/bin/env python3
"""Aula 13a: modo seguro passa; cada falha e detectada pelo ASan.

Compila 13a_seguranca.cpp com ASan/UBSan, roda o caminho seguro e os tres
modos de falha em processos separados, exigindo o relatorio certo.
"""
import subprocess
import sys
import tempfile
from pathlib import Path

FONTE = Path(__file__).with_name("13a_seguranca.cpp")
ESPERADO = {
    "overflow": "heap-buffer-overflow",
    "uaf": "heap-use-after-free",
    "doublefree": "double-free",
}


def main() -> int:
    with tempfile.TemporaryDirectory() as d:
        exe = str(Path(d) / "seg")
        c = subprocess.run(
            ["g++", "-std=c++20", "-O1", "-Wall", "-Wextra", "-Wpedantic",
             "-Werror", "-fsanitize=address,undefined",
             "-fno-sanitize-recover=all", str(FONTE), "-o", exe],
            capture_output=True, text=True, timeout=300)
        if c.returncode != 0:
            print("FALHA compilacao:\n" + c.stderr)
            return 1
        ok = subprocess.run([exe], capture_output=True, text=True, timeout=30)
        assert ok.returncode == 0 and "seguro ok" in ok.stdout, ok.stdout + ok.stderr
        print("ok - caminho seguro")
        for modo, trecho in ESPERADO.items():
            r = subprocess.run([exe, modo], capture_output=True, text=True, timeout=30)
            assert r.returncode != 0, f"{modo} deveria abortar"
            assert trecho in r.stderr, f"{modo}: esperado {trecho}\n{r.stderr[:500]}"
            print(f"ok - {modo}: ASan reportou {trecho}")
    print("TEST_13A_OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
