"""Busca binaria com invariante documentado e medicao linear x binaria."""

import time


def busca_binaria(valores: list[int], alvo: int) -> int:
    """Devolve o indice de alvo em valores ou -1 se ausente.

    Pre-condicao: valores ordenada em ordem nao decrescente.
    Invariante: se alvo esta na lista, esta em valores[ini:fim].
    """
    ini, fim = 0, len(valores)
    while ini < fim:
        meio = (ini + fim) // 2
        if valores[meio] == alvo:
            return meio
        if alvo < valores[meio]:
            fim = meio
        else:
            ini = meio + 1
    return -1


def busca_linear(valores: list[int], alvo: int) -> int:
    """Versao de referencia: percorre tudo, sem exigir ordenacao."""
    for i, valor in enumerate(valores):
        if valor == alvo:
            return i
    return -1


def medir(funcao, valores: list[int], alvo: int, repeticoes: int) -> float:
    inicio = time.perf_counter()
    for _ in range(repeticoes):
        funcao(valores, alvo)
    return (time.perf_counter() - inicio) / repeticoes


def main() -> None:
    assert busca_binaria([], 5) == -1
    assert busca_binaria([5], 5) == 0
    assert busca_binaria([5], 4) == -1
    assert busca_binaria([1, 3, 5, 7], 1) == 0
    assert busca_binaria([1, 3, 5, 7], 7) == 3
    assert busca_binaria([1, 3, 5, 7], 4) == -1
    assert busca_binaria([2, 2, 2], 2) in (0, 1, 2)

    grande = list(range(0, 2_000_000, 2))
    alvo_presente, alvo_ausente = 1_999_998, 1_999_999
    for f in (busca_linear, busca_binaria):
        assert f(grande, alvo_presente) == alvo_presente // 2
        assert f(grande, alvo_ausente) == -1

    rep_lin, rep_bin = 5, 200
    t_lin = medir(busca_linear, grande, alvo_ausente, rep_lin)
    t_bin = medir(busca_binaria, grande, alvo_ausente, rep_bin)
    print(f"linear: {t_lin * 1e3:.3f} ms/media ({rep_lin}x)")
    print(f"binaria: {t_bin * 1e3:.6f} ms/media ({rep_bin}x)")


if __name__ == "__main__":
    main()
