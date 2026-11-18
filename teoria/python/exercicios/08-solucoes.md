---
layout: default
---

# Aula 08 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../08.md#17-exercicios-obrigatorios).

```python
class SenhaInvalida(Exception):
    pass


def para_inteiro(texto: str) -> int | None:
    try:
        return int(texto)
    except ValueError:
        return None


def dividir(a: float, b: float) -> float:
    if b == 0:
        raise ZeroDivisionError("divisao por zero")
    return a / b


def validar_senha(senha: str) -> None:
    if len(senha) < 8:
        raise SenhaInvalida("senha curta demais")


def converter_estrito(texto: str) -> int:
    try:
        return int(texto)
    except ValueError as original:
        raise SenhaInvalida(f"valor nao numerico: {texto!r}") from original


def main() -> None:
    assert para_inteiro("42") == 42 and para_inteiro("abc") is None
    assert dividir(10, 2) == 5.0

    eventos: list[str] = []
    try:
        dividir(1, 0)
    except ZeroDivisionError:
        eventos.append("except")
    else:
        eventos.append("else")
    finally:
        eventos.append("finally")
    try:
        dividir(4, 2)
    except ZeroDivisionError:
        eventos.append("except2")
    else:
        eventos.append("else2")
    finally:
        eventos.append("finally2")
    print("ordem try/except/else/finally:", eventos)
    assert eventos == ["except", "finally", "else2", "finally2"]

    try:
        validar_senha("123")
    except SenhaInvalida as e:
        print("excecao propria:", e)

    try:
        converter_estrito("xyz")
    except SenhaInvalida as e:
        print("encadeada, causa preservada:", type(e.__cause__).__name__)
        assert isinstance(e.__cause__, ValueError)
    print("testes ok")


if __name__ == "__main__":
    main()
```

1. `para_inteiro` devolve `None` na falha porque o exercicio pede conversao tolerante; onde a falha e defeito do programa, o desenho certo e deixar o `ValueError` subir, nao disfarca-lo de `None` — a escolha se escreve, nao se herda.

**Autoverificacao:** conversao tolerante, divisao protegida, ordem dos quatro blocos observada e `__cause__` preservando o `ValueError` original.
