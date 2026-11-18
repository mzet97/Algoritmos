---
layout: default
---

# Aula 02 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../02.md#11-exercicios-obrigatorios).

```python
def classificar(nota: float) -> str:
    if nota >= 7.0:
        return "Aprovado"
    if nota >= 5.0:
        return "Recuperacao"
    return "Reprovado"


def eh_par(n: int) -> bool:
    return n % 2 == 0


def maior(a: float, b: float) -> float:
    return a if a >= b else b


def main() -> None:
    assert classificar(8.5) == "Aprovado"
    assert classificar(6.0) == "Recuperacao"
    assert classificar(3.0) == "Reprovado"
    print([classificar(n) for n in (8.5, 6.0, 3.0)])

    crescente = [n for n in range(1, 11)]
    regressiva = []
    n = 5
    while n >= 1:
        regressiva.append(n)
        n -= 1
    print(crescente)
    print(regressiva)
    assert crescente == list(range(1, 11)) and regressiva == [5, 4, 3, 2, 1]

    assert eh_par(4) and not eh_par(7)
    assert maior(3, 9) == 9 and maior(9, 3) == 9 and maior(5, 5) == 5
    print("testes ok")


if __name__ == "__main__":
    main()
```

1. As faixas se testam na fronteira: 7.0 aprova, 5.0 recupera, 4.99 reprova — e onde o exercicio mora, nao no meio da faixa.

**Autoverificacao:** fronteiras classificadas, as duas contagens exatas e `maior` empatado devolvendo o proprio valor.
