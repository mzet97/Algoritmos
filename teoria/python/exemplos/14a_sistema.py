"""Do interpretador ao kernel: bytecode, arquivo, processo e memoria."""

import dis
import hashlib
import os
import subprocess
import sys
import time
import tracemalloc


def soma_quadrados(n: int) -> int:
    total = 0
    for i in range(n):
        total += i * i
    return total


def bytecode_demo() -> None:
    print("== bytecode de soma_quadrados ==")
    dis.dis(soma_quadrados)


def ler_em_blocos(caminho: str, bloco: int = 65536) -> tuple[int, float]:
    inicio = time.perf_counter()
    total = 0
    with open(caminho, "rb") as f:
        while True:
            parte = f.read(bloco)
            if not parte:
                break
            total += len(parte)
    return total, time.perf_counter() - inicio


def memoria_processo() -> dict:
    info = {}
    try:
        import resource
        uso = resource.getrusage(resource.RUSAGE_SELF)
        info["maxrss_kb"] = uso.ru_maxrss
        info["minflt"] = uso.ru_minflt
        info["majflt"] = uso.ru_majflt
    except ImportError:
        info["resource"] = "indisponivel (nao e Linux/macOS)"
    atual, pico = tracemalloc.get_traced_memory()
    info["tracemalloc_atual_b"] = atual
    info["tracemalloc_pico_b"] = pico
    try:
        info["pagina_b"] = os.sysconf("SC_PAGE_SIZE")
    except (AttributeError, ValueError, OSError):
        info["pagina_b"] = "desconhecida"
    return info


def main() -> int:
    print(f"implementacao={sys.implementation.name} versao={sys.version.split()[0]}")
    bytecode_demo()

    tracemalloc.start()
    dados_teste = bytes(range(256)) * 4096  # 1 MiB deterministico
    assert hashlib.sha256(dados_teste).hexdigest() == hashlib.sha256(bytes(range(256)) * 4096).hexdigest()
    caminho = "/tmp/14a_dados.bin"
    with open(caminho, "wb") as f:
        f.write(dados_teste)
    total, dur = ler_em_blocos(caminho)
    assert total == len(dados_teste)
    print(f"lidos {total} bytes em {dur * 1e3:.2f} ms")

    # Processo filho: a mesma contabilidade do C 16a, vista do Python.
    r = subprocess.run([sys.executable, "--version"], capture_output=True, text=True, timeout=10)
    assert r.returncode == 0, r.stderr
    print("filho:", r.stdout.strip() or r.stderr.strip())

    # Falhas provocadas: arquivo ausente e diretorio como arquivo.
    for ruim in ("/tmp/14a_nao_existe.bin", "/tmp"):
        try:
            ler_em_blocos(ruim)
            print(f"INESPERADO: {ruim} abriu")
            return 2
        except (FileNotFoundError, PermissionError, IsADirectoryError, OSError) as e:
            print(f"falha contida ({type(e).__name__}): {ruim}")

    for k, v in memoria_processo().items():
        print(f"{k}={v}")
    tracemalloc.stop()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
