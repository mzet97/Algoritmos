---
layout: default
---

# Aula 03 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../03.md#12-exercicios-obrigatorios).

```python
import copy


def acrescenta(lista: list[int], valor: int) -> None:
    lista.append(valor)


def apenas_rebinding(lista: list[int]) -> None:
    lista = [999]  # so o nome local aponta para outra lista


def main() -> None:
    a = [1, 2, 3]
    b = a
    b.append(4)
    print("mesmo objeto:", a is b, "| a mudou junto:", a == [1, 2, 3, 4])
    assert a == [1, 2, 3, 4] and a is b

    x = [1, 2]
    y = [1, 2]
    print("== compara valor:", x == y, "| is compara identidade:", x is y)
    assert x == y and x is not y

    original = [[1, 2], [3, 4]]
    rasa = original.copy()
    rasa[0].append(99)
    print("copia rasa falhou na aninhada:", original == [[1, 2, 99], [3, 4]])
    assert original == [[1, 2, 99], [3, 4]]

    original2 = [[1, 2], [3, 4]]
    profunda = copy.deepcopy(original2)
    profunda[0].append(99)
    print("deepcopy isolou:", original2 == [[1, 2], [3, 4]])
    assert original2 == [[1, 2], [3, 4]]

    dados = [1, 2]
    acrescenta(dados, 3)
    apenas_rebinding(dados)
    print("mutacao atravessou, rebinding nao:", dados == [1, 2, 3])
    assert dados == [1, 2, 3]
    print("testes ok")


if __name__ == "__main__":
    main()
```

2 e 5 sao a mesma licao em dois trajes: nome, objeto e valor sao tres coisas. Quem confunde `is` com `==` ou rebinding com mutacao depura o programa errado.

**Autoverificacao:** alias mudando junto, `==` sem `is`, rasa falhando onde a profunda isola e mutacao atravessando a chamada.
