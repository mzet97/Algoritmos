---
layout: default
---

# Aula 17 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../17.md#18-exercicios-obrigatorios).

```python
import cProfile
import io
import pstats
import sys
import time
import timeit


def soma_lista(n: int) -> int:
    return sum([i * i for i in range(n)])


def soma_generator(n: int) -> int:
    return sum(i * i for i in range(n))


def trabalho() -> int:
    total = 0
    for i in range(20000):
        total += i * i
    return total


class SemSlots:
    def __init__(self) -> None:
        self.a, self.b = 1, 2


class ComSlots:
    __slots__ = ("a", "b")

    def __init__(self) -> None:
        self.a, self.b = 1, 2


def main() -> None:
    n = 20000
    assert soma_lista(n) == soma_generator(n)  # igualdade antes do relogio

    inicio = time.perf_counter()
    soma_lista(n)
    t_lista = time.perf_counter() - inicio
    inicio = time.perf_counter()
    soma_generator(n)
    t_gen = time.perf_counter() - inicio
    print(f"perf_counter: lista={t_lista * 1000:.1f}ms generator={t_gen * 1000:.1f}ms")

    melhor = timeit.timeit(lambda: soma_generator(2000), number=50)
    print(f"timeit (50x soma_generator(2000)): {melhor:.3f}s")

    perfil = cProfile.Profile()
    perfil.enable()
    trabalho()
    perfil.disable()
    saida = io.StringIO()
    pstats.Stats(perfil, stream=saida).sort_stats("cumulative").print_stats(5)
    topo = [linha for linha in saida.getvalue().splitlines() if "trabalho" in linha]
    print("hotspot no topo do perfil:", bool(topo))

    sem, com = SemSlots(), ComSlots()
    print("getsizeof sem/com slots:", sys.getsizeof(sem), sys.getsizeof(com),
          "| sem tem __dict__:", hasattr(sem, "__dict__"), "| com tem:", hasattr(com, "__dict__"))
    assert hasattr(sem, "__dict__") and not hasattr(com, "__dict__")
    print("testes ok")


if __name__ == "__main__":
    main()
```

5. O trade-off dos `__slots__`: some o `__dict__` por instancia (menos memoria, sem atributos novos dinamicos) e heranca/serializadores que esperam `__dict__` estranham. Em classe de dominio com campos estaveis e milhoes de instancias, paga; em classe exploratoria, atrapalha.

4. O generator neste exemplo nao economiza tempo — economiza **memoria**: a lista de 20000 quadrados nunca existe inteira. Quando o consumidor para no meio, ele economiza o resto do trabalho tambem.

**Autoverificacao:** igualdade antes do relogio, as duas medicoes feitas, hotspot nomeado pelo perfil e `__dict__` presente so onde deveria.
