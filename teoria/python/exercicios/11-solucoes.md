---
layout: default
---

# Aula 11 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../11.md#18-exercicios-obrigatorios). Anotacoes sao verificadas em runtime aqui apenas onde o Python permite; o contrato completo e do verificador de tipos (`mypy`/`pyright`) no seu projeto:

```python
from collections.abc import Iterable
from typing import Protocol, TypedDict

Ponto = tuple[float, float]


class Produto(TypedDict):
    nome: str
    preco: float


class Resumivel(Protocol):
    def resumo(self) -> str: ...


def media(a: float, b: float) -> float:
    return (a + b) / 2.0


def juntar(partes: Iterable[str]) -> str:
    return "-".join(partes)


class Relatorio:
    def resumo(self) -> str:
        return "relatorio resumido"


def mostrar(item: Resumivel) -> str:
    return item.resumo()


def distancia_origem(p: Ponto) -> float:
    return (p[0] ** 2 + p[1] ** 2) ** 0.5


def main() -> None:
    assert media(3.0, 5.0) == 4.0
    assert juntar(["a", "b"]) == "a-b"
    assert juntar(x for x in ("p", "q")) == "p-q"  # generator tambem e Iterable

    produto: Produto = {"nome": "livro", "preco": 59.9}
    assert produto["nome"] == "livro"

    # Relatorio nao herda de Resumivel: o Protocol e estrutural.
    assert mostrar(Relatorio()) == "relatorio resumido"
    assert distancia_origem((3.0, 4.0)) == 5.0
    print("testes ok")


if __name__ == "__main__":
    main()
```

5. `Protocol` e o contrato sem casamento: `Relatorio` serve porque **tem** `resumo()`, nao porque declarou parentesco. E assim que se tipa codigo de terceiros que voce nao pode fazer herdar das suas classes.

**Autoverificacao:** media anotada, `Iterable` aceitando generator, `TypedDict` em uso, alias aplicado e `Protocol` aceitando quem nao herda dele.
