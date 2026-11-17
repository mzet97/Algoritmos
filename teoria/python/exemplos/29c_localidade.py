"""Aula 29c — localidade e Amdahl em Python: onde a aceleracao para."""
import math
import os
import time
from multiprocessing import Pool


def trabalho(valores: list[float]) -> float:
    return sum(math.sin(x) * math.cos(x) for x in valores)


def main() -> int:
    n = 3_000_000
    dados = [i / 1000.0 for i in range(n)]

    t0 = time.perf_counter()
    ref = trabalho(dados)
    t_seq = time.perf_counter() - t0

    # Referencia conferida antes de cronometrar qualquer paralelo.
    esperado = trabalho(dados[:1000])
    assert abs(trabalho(dados[:1000]) - esperado) == 0

    cpus = min(8, os.cpu_count() or 1)  # pool limitado: 24 processos pagam criacao
    blocos = [dados[i::cpus] for i in range(cpus)]
    # Stride propositalmente ruim para memoria? Nao: fatias contiguas.
    blocos = [dados[i * n // cpus:(i + 1) * n // cpus] for i in range(cpus)]
    t0 = time.perf_counter()
    with Pool(cpus) as pool:
        partes = pool.map(trabalho, blocos)
    t_par = time.perf_counter() - t0
    assert abs(sum(partes) - ref) < 1e-6 * abs(ref)
    speedup = t_seq / t_par
    print(f"seq={t_seq:.3f}s processos({cpus})={t_par:.3f}s speedup={speedup:.2f}x")
    assert t_par < t_seq

    # Amdahl: com fracao paralelizavel p, o teto e 1/(1-p). Medimos a
    # fracao de tempo fora do pool (montar blocos + somar partes) e
    # mostramos o teto que ela impoe, em vez de prometer cpus x.
    def amdahl(p: float, s: int) -> float:
        return 1.0 / ((1 - p) + p / s)

    for p in (0.5, 0.9, 0.99):
        print(f"p={p}: teto com {cpus} nucleos = {amdahl(p, cpus):.2f}x "
              f"(infinito nucleos = {1 / (1 - p):.1f}x)")
    assert amdahl(0.9, cpus) < cpus  # o termo sequencial sempre morde

    # Caso em que acelerar perde: trabalho minusculo, pool inteiro.
    pequeno = [0.5] * 200
    t0 = time.perf_counter()
    trabalho(pequeno)
    t_micro_seq = time.perf_counter() - t0
    t0 = time.perf_counter()
    with Pool(4) as pool:
        pool.map(trabalho, [pequeno[i::4] for i in range(4)])
    t_micro_par = time.perf_counter() - t0
    print(f"micro: seq={t_micro_seq * 1000:.2f}ms pool={t_micro_par * 1000:.1f}ms "
          f"(paralelo perde por criacao de processos)")
    assert t_micro_par > t_micro_seq
    print("29c ok: referencia, speedup medido, Amdahl calculado e o caso que perde")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
