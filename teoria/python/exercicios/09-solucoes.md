---
layout: default
---

# Aula 09 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../09.md#17-exercicios-obrigatorios).

```python
def multiplos_de_tres(n: int):
    for i in range(1, n + 1):
        yield 3 * i


class Contador:
    def __init__(self, limite: int) -> None:
        self.atual, self.limite = 0, limite

    def __iter__(self):
        return self

    def __next__(self) -> int:
        if self.atual >= self.limite:
            raise StopIteration
        self.atual += 1
        return self.atual


def main() -> None:
    it = iter([10, 20, 30])
    assert (next(it), next(it)) == (10, 20)
    try:
        while True:
            next(it)
    except StopIteration:
        print("StopIteration encerra o consumo manual")

    quadrados = [n * n for n in range(5)]
    cubos = {n: n ** 3 for n in range(4)}
    assert quadrados == [0, 1, 4, 9, 16] and cubos == {0: 0, 1: 1, 2: 8, 3: 27}

    assert list(multiplos_de_tres(4)) == [3, 6, 9, 12]
    assert list(Contador(3)) == [1, 2, 3]

    # Pipeline lazy: dobrar, depois filtrar pares do resultado.
    pipeline = (x for x in (n * 2 for n in range(10)) if x % 4 == 0)
    assert list(pipeline) == [0, 4, 8, 12, 16]
    print("testes ok")


if __name__ == "__main__":
    main()
```

6. Nada no pipeline executou ate o `list(...)`: generator e uma promessa de trabalho, nao o trabalho — e por isso ele atravessa arquivos e fluxos grandes sem materializar, como a [aula 13](13-solucoes.md) cobra.

**Autoverificacao:** consumo manual com fim explicito, comprehensions exatas, iterador de classe sem `yield` e pipeline de dois generators.
