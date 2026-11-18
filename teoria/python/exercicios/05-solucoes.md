---
layout: default
---

# Aula 05 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../05.md#11-exercicios-obrigatorios).

```python
from functools import wraps


def saudar(nome: str, pontuacao: str = "!") -> str:
    return f"Ola, {nome}{pontuacao}"


def acrescenta_bug(item: int, lista: list[int] = []) -> list[int]:
    lista.append(item)
    return lista


def acrescenta_ok(item: int, lista: list[int] | None = None) -> list[int]:
    if lista is None:
        lista = []
    lista.append(item)
    return lista


def media(*args: float) -> float:
    if not args:
        raise ValueError("media de zero valores")
    return sum(args) / len(args)


def criar_potencia(expoente: int):
    def potencia(base: float) -> float:
        return base ** expoente
    return potencia


def anunciar(func):
    @wraps(func)
    def embrulho(*args, **kwargs):
        print("inicio")
        resultado = func(*args, **kwargs)
        print("fim")
        return resultado
    return embrulho


@anunciar
def dobrar(n: int) -> int:
    return n * 2


def main() -> None:
    assert saudar("Ada") == "Ola, Ada!" and saudar("Ada", ".") == "Ola, Ada."

    # O bug, reproduzido de proposito: duas chamadas "independentes" dividem a lista.
    print("bug:", acrescenta_bug(1), acrescenta_bug(2))
    assert acrescenta_bug(3) == [1, 2, 3]
    assert acrescenta_ok(1) == [1] and acrescenta_ok(2) == [2]

    assert media(2, 4, 6) == 4.0
    try:
        media()
    except ValueError:
        print("media vazia: erro explicito, nao divisao por zero disfarçada")

    quadrado, cubo = criar_potencia(2), criar_potencia(3)
    assert quadrado(5) == 25 and cubo(2) == 8
    assert dobrar(21) == 42 and dobrar.__name__ == "dobrar"
    print("testes ok")


if __name__ == "__main__":
    main()
```

2. O padrao mutavel e avaliado uma vez, quando o `def` executa: todas as chamadas sem o argumento dividem o mesmo objeto. `None` como sentinela e a correcao padrao porque cria a lista **por chamada**.

**Autoverificacao:** padrao usado e trocado, bug reproduzido e corrigido lado a lado, media vazia com erro proprio e decorator preservando nome com `@wraps`.
