"""Concorrencia em Python: corrida demonstrada, Lock provado, medidas honestas."""

import asyncio
import sys
import threading
import time
from concurrent import futures


def soma_fatia(par: tuple) -> int:
    """Soma i*i no intervalo [a, b). Nivel de modulo: picklable para processos."""
    a, b = par
    return sum(i * i for i in range(a, b))


def corrida_sem_lock(threads: int = 8, repeticoes: int = 100_000) -> int:
    total = 0

    def incrementar() -> None:
        nonlocal total
        for _ in range(repeticoes):
            atual = total
            # sleep(0) cede o GIL ENTRE a leitura e a escrita: outra
            # thread entra no meio e escreve por cima. Sem esse ponto de
            # troca, a sequencia curta raramente e interrompida.
            time.sleep(0)
            total = atual + 1  # escreve sobre valor velho: perde atualizacoes

    ts = [threading.Thread(target=incrementar) for _ in range(threads)]
    for t in ts:
        t.start()
    for t in ts:
        t.join()
    return total


def corrida_com_lock(threads: int = 8, repeticoes: int = 100_000) -> int:
    total = 0
    trava = threading.Lock()

    def incrementar() -> None:
        nonlocal total
        for _ in range(repeticoes):
            with trava:
                total += 1

    ts = [threading.Thread(target=incrementar) for _ in range(threads)]
    for t in ts:
        t.start()
    for t in ts:
        t.join()
    return total


def medir(funcao, *args):
    inicio = time.perf_counter()
    resultado = funcao(*args)
    return resultado, time.perf_counter() - inicio


async def tarefa_io(nome: str, espera: float) -> str:
    await asyncio.sleep(espera)  # cooperativo: nao bloqueia o loop
    return nome


async def demo_asyncio() -> list:
    return await asyncio.gather(
        *(tarefa_io(f"t{i}", 0.05) for i in range(20)),
    )


def main() -> int:
    print(f"versao={sys.version.split()[0]} gil={getattr(sys, '_is_gil_enabled', lambda: 'n/a (<=3.12)')()}")
    esperado = 8 * 100_000

    sem_lock, t_sem = medir(corrida_sem_lock, 8, 20_000)
    print(f"sem lock: total={sem_lock} (esperado {8 * 20_000}) em {t_sem:.2f}s -- ilustrativo, nao contrato")
    com_lock, t_lock = medir(corrida_com_lock)
    assert com_lock == esperado, com_lock
    print(f"com lock: total={com_lock} em {t_lock:.2f}s -- contrato cumprido")

    # CPU-bound: sequencial x threads x processos (mesmo resultado, custos diferentes).
    n, partes = 1_000_000, 4
    fatias = [(i * n // partes, (i + 1) * n // partes) for i in range(partes)]
    esperado_soma = sum(i * i for i in range(n))

    t0 = time.perf_counter()
    seq = sum(soma_fatia(ab) for ab in fatias)
    t_seq = time.perf_counter() - t0

    with futures.ThreadPoolExecutor(max_workers=4) as exe:
        t0 = time.perf_counter()
        par_t = sum(exe.map(soma_fatia, fatias))
        t_thr = time.perf_counter() - t0

    with futures.ProcessPoolExecutor(max_workers=4) as exe:
        t0 = time.perf_counter()
        par_p = sum(exe.map(soma_fatia, fatias))
        t_pro = time.perf_counter() - t0

    assert seq == par_t == par_p == esperado_soma, (seq, par_t, par_p)
    print(f"cpu seq={t_seq:.3f}s threads={t_thr:.3f}s processos={t_pro:.3f}s (mesmo resultado)")

    # Asyncio: 20 tarefas de 50 ms em ~50 ms totais, mais timeout que funciona.
    t0 = time.perf_counter()
    nomes = asyncio.run(demo_asyncio())
    t_async = time.perf_counter() - t0
    assert sorted(nomes) == sorted(f"t{i}" for i in range(20)), nomes
    print(f"asyncio: 20 tarefas em {t_async * 1e3:.1f} ms")

    try:
        asyncio.run(asyncio.wait_for(asyncio.sleep(5), timeout=0.1))
        print("INESPERADO: timeout nao disparou")
        return 2
    except TimeoutError:
        print("timeout contido: TimeoutError em 0.1s")

    # Processo filho com codigo de saida e sem orfaos.
    import subprocess
    p = subprocess.run([sys.executable, "--version"], capture_output=True, text=True, timeout=10)
    assert p.returncode == 0, p.stderr
    print("filho:", (p.stdout or p.stderr).strip())
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
