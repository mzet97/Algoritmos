"""Aula 14b — cache e pagina vistos de Python: localidade em listas."""
import os
import resource
import time


def main() -> int:
    n = 900
    # Lista de listas: linhas sao objetos separados; "matriz" em Python
    # nao e um bloco contiguo como em C — a aula mede exatamente isso.
    m = [[(i * n + j) % 251 for j in range(n)] for i in range(n)]

    t0 = time.perf_counter()
    soma_linhas = 0
    for linha in m:
        soma_linhas += sum(linha)
    t_linhas = time.perf_counter() - t0

    t0 = time.perf_counter()
    soma_colunas = 0
    for j in range(n):
        for i in range(n):
            soma_colunas += m[i][j]
    t_colunas = time.perf_counter() - t0

    assert soma_linhas == soma_colunas
    print(f"linhas={t_linhas:.4f}s colunas={t_colunas:.4f}s "
          f"razao={t_colunas / t_linhas:.2f}x")
    assert t_colunas > t_linhas

    # Pagina e memoria do processo pelo SO, nao pelo tracemalloc.
    pagina = os.sysconf("SC_PAGE_SIZE")
    rss_kb = resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
    print(f"pagina={pagina} bytes rss_max={rss_kb} KiB "
          f"(ru_maxrss e pico, nao residencia atual)")
    assert pagina in (4096, 16384, 65536)

    # Vetor plano (array('l')): um bloco so, percurso por passo simulando
    # colunas com stride — o contraste com a lista de listas e a aula.
    from array import array

    plano = array("l", [0]) * (n * n)
    for i in range(n * n):
        plano[i] = i % 251
    t0 = time.perf_counter()
    s1 = 0
    for i in range(n):
        base = i * n
        for j in range(n):
            s1 += plano[base + j]
    t_seq = time.perf_counter() - t0
    t0 = time.perf_counter()
    s2 = 0
    for j in range(n):
        for i in range(n):
            s2 += plano[i * n + j]
    t_stride = time.perf_counter() - t0
    assert s1 == s2 == soma_linhas
    print(f"array plano: sequencial={t_seq:.4f}s stride={t_stride:.4f}s "
          f"razao={t_stride / t_seq:.2f}x")
    print("14b ok: localidade medida nas duas representacoes, pagina lida do SO")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
