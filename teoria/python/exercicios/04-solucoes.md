---
layout: default
---

# Aula 04 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../04.md#11-exercicios-obrigatorios).

```python
def main() -> None:
    palavra = "algoritmo"
    print(palavra[0], palavra[-1], palavra[2:5])
    assert (palavra[0], palavra[-1], palavra[2:5]) == ("a", "o", "gor")

    numeros = [1, 2, 3]
    numeros.append(4)
    numeros.remove(2)
    invertida = numeros[::-1]
    print(numeros, invertida)
    assert numeros == [1, 3, 4] and invertida == [4, 3, 1]

    coordenadas = (10, 20)
    x, y = coordenadas
    assert (x, y) == (10, 20)

    livro = {"titulo": "Sistemas", "autor": "Ada", "ano": 2026}
    print(livro["titulo"], livro["ano"])
    assert livro["autor"] == "Ada"

    unicos = set([1, 2, 2, 3, 3, 3])
    print(sorted(unicos))
    assert unicos == {1, 2, 3}

    vogais = set("aeiou")
    resultado = {letra: letra in vogais for letra in "casa"}
    print(resultado)
    assert resultado == {"c": False, "a": True, "s": False}
    print("testes ok")


if __name__ == "__main__":
    main()
```

5. O `set` nao promete ordem de iteracao; `sorted(unicos)` aparece no print porque **ordenar** e a maneira honesta de mostrar conjunto em texto.

**Autoverificacao:** fatias exatas, lista invertida como objeto novo, desempacotamento nomeado e vogais classificadas letra a letra.
