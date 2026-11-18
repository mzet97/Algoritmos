---
layout: default
---

# Aula 10 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../10.md#14-exercicios-obrigatorios).

```python
from contextlib import contextmanager


class Produto:
    def __init__(self, nome: str, preco: float) -> None:
        self.nome, self.preco = nome, preco

    def __repr__(self) -> str:
        return f"Produto(nome={self.nome!r}, preco={self.preco!r})"

    def __str__(self) -> str:
        return f"{self.nome} (R$ {self.preco:.2f})"


class Prateleira:
    def __init__(self) -> None:
        self._itens: list[str] = []

    def adicionar(self, item: str) -> None:
        self._itens.append(item)

    def __len__(self) -> int:
        return len(self._itens)

    def __iter__(self):
        return iter(self._itens)

    def __contains__(self, item: str) -> bool:
        return item in self._itens


class Multiplicador:
    def __init__(self, fator: int) -> None:
        self.fator = fator

    def __call__(self, valor: int) -> int:
        return valor * self.fator


class RecursoManual:
    def __init__(self, eventos: list[str]) -> None:
        self.eventos = eventos

    def __enter__(self) -> str:
        self.eventos.append("abriu")
        return "recurso"

    def __exit__(self, tipo, valor, rastro) -> bool:
        self.eventos.append("fechou")
        return False


@contextmanager
def recurso_funcional(eventos: list[str]):
    eventos.append("abriu")
    try:
        yield "recurso"
    finally:
        eventos.append("fechou")


def main() -> None:
    p = Produto("livro", 59.9)
    assert repr(p) == "Produto(nome='livro', preco=59.9)"
    assert str(p) == "livro (R$ 59.90)"

    prateleira = Prateleira()
    prateleira.adicionar("a")
    prateleira.adicionar("b")
    assert len(prateleira) == 2 and "a" in prateleira and list(prateleira) == ["a", "b"]
    assert Multiplicador(3)(7) == 21

    eventos1: list[str] = []
    with RecursoManual(eventos1) as recurso:
        assert recurso == "recurso"
    eventos2: list[str] = []
    with recurso_funcional(eventos2) as recurso:
        assert recurso == "recurso"
    print("manual e funcional, mesma ordem:", eventos1 == eventos2 == ["abriu", "fechou"])
    assert eventos1 == eventos2 == ["abriu", "fechou"]
    print("testes ok")


if __name__ == "__main__":
    main()
```

1. `repr` bom reconstroi o objeto (ou quase); `str` bom se le em um relatorio. Quando so um existe, o Python usa o `repr` para os dois papis — por isso o exercicio pede os dois escritos.

**Autoverificacao:** repr/str distintos, container respondendo aos tres protocolos, instancia chamavel e os dois contextos com a mesma ordem de eventos.
