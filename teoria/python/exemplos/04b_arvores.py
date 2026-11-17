"""Aula 04b — arvores e ordenacao em Python: BST, degeneracao e sorted."""
import random
import sys
import time


class No:
    __slots__ = ("chave", "esq", "dir")

    def __init__(self, chave):
        self.chave = chave
        self.esq = None
        self.dir = None


def inserir(raiz, chave):
    if raiz is None:
        return No(chave), True
    if chave == raiz.chave:
        return raiz, False
    if chave < raiz.chave:
        raiz.esq, ok = inserir(raiz.esq, chave)
    else:
        raiz.dir, ok = inserir(raiz.dir, chave)
    return raiz, ok


def em_ordem(raiz, saida):
    if raiz is None:
        return
    em_ordem(raiz.esq, saida)
    saida.append(raiz.chave)
    em_ordem(raiz.dir, saida)


def altura(raiz):
    if raiz is None:
        return 0
    return 1 + max(altura(raiz.esq), altura(raiz.dir))


def main() -> int:
    rng = random.Random(42)
    for _ in range(300):
        valores = [rng.randint(-20, 19) for _ in range(rng.randrange(60))]
        arv = None
        distintas = 0
        for v in valores:
            arv, ok = inserir(arv, v)
            distintas += 1 if ok else 0
        saida = []
        em_ordem(arv, saida)
        assert saida == sorted(set(valores)), "em-ordem diverge do oraculo"
        assert len(saida) == distintas
    print("propriedade ok: BST em-ordem == sorted(set) em 300 casos")

    sys.setrecursionlimit(10000)
    lista = None
    for i in range(300):
        lista, _ = inserir(lista, i)
    assert altura(lista) == 300
    perm = list(range(1000))
    rng.shuffle(perm)
    emb = None
    for v in perm:
        emb, _ = inserir(emb, v)
    h = altura(emb)
    print(f"altura ordenada(300)={300} embaralhada(1000)={h}")
    assert h < 100

    grande = [rng.randrange(10**6) for _ in range(200_000)]
    t0 = time.perf_counter()
    ordenado = sorted(grande)
    t_sorted = time.perf_counter() - t0
    assert ordenado == sorted(grande)
    print(f"sorted(200k) em {t_sorted:.3f}s (Timsort C, sem no por elemento)")
    print("04b ok: BST, degeneracao medida e o preco de no em Python puro")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
