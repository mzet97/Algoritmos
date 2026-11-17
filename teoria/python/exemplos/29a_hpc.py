"""Baseline escalar, NumPy, threads, processos e a conta da GPU."""

import os
import time
from concurrent import futures

try:
    import numpy as np
except ImportError:  # pragma: no cover - ambiente sem numpy
    raise SystemExit("instale numpy: pip install numpy")


def referencia(xs):
    """Baseline legivel: soma de quadrados, O(n) com constante alta."""
    total = 0
    for x in xs:
        total += x * x
    return total


def medir(funcao, *args, repeticoes=5):
    melhor = float("inf")
    resultado = None
    for _ in range(repeticoes):
        inicio = time.perf_counter()
        resultado = funcao(*args)
        dur = time.perf_counter() - inicio
        if dur < melhor:
            melhor = dur
    return resultado, melhor


def soma_numpy(bloco):
    return int((bloco * bloco).sum())


def soma_fatia(par):
    a, b = par
    return sum(i * i for i in range(a, b))


def main():
    n = 1_000_000
    nbytes = n * 8
    print(f"n={n} bytes={nbytes} cpus={os.cpu_count()}")

    xs = list(range(n))
    arr = np.arange(n, dtype="int64")

    esperado, t_ref = medir(referencia, xs, repeticoes=3)
    print(f"referencia: {t_ref * 1e3:.1f} ms")

    r_np, t_np = medir(soma_numpy, arr, repeticoes=5)
    assert r_np == esperado, (r_np, esperado)
    print(f"numpy: {t_np * 1e3:.3f} ms (x{t_ref / t_np:.0f} contra referencia)")

    fatias = [(i * n // 4, (i + 1) * n // 4) for i in range(4)]
    partes = [(arr[a:b]) for a, b in fatias]
    with futures.ThreadPoolExecutor(max_workers=4) as exe:
        def roda_threads():
            return sum(exe.map(soma_numpy, partes))
        r_thr, t_thr = medir(roda_threads, repeticoes=3)
    assert r_thr == esperado, r_thr
    print(f"threads+numpy: {t_thr * 1e3:.3f} ms (x{t_ref / t_thr:.0f})")

    with futures.ProcessPoolExecutor(max_workers=4) as exe:
        def roda_processos():
            return sum(exe.map(soma_fatia, fatias))
        t0 = time.perf_counter()
        r_pro = roda_processos()
        t_pro = time.perf_counter() - t0
    assert r_pro == esperado, r_pro
    print(f"processos: {t_pro * 1e3:.1f} ms (mesmo resultado, paga spawn+IPC)")

    # Conta da GPU: aritmetica deterministica, sem hardware.
    banda_gbs = 16.0
    t_ida = (nbytes / 1024**3) / banda_gbs
    # Ida e volta com o mesmo volume, kernel hipotetico de 1 ms:
    t_kernel = 0.001
    t_total = 2 * t_ida + t_kernel
    print(f"viagem: ida+volta={2 * t_ida * 1e3:.2f} ms kernel=1.00 ms total={t_total * 1e3:.2f} ms")
    print(f"numpy local: {t_np * 1e3:.3f} ms")
    if t_total < t_np:
        print("veredito: a GPU hipotetica venceria NESTE cenario")
    else:
        print("veredito: nem kernel instantaneo compensaria sem banda maior; fique na CPU")
    assert t_ida > 0
    print("modelo ok: referencia, multicore medido e conta da transferencia")


if __name__ == "__main__":
    raise SystemExit(main())
