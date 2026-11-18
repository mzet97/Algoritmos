---
layout: default
---

# Aula 01 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../01.md#11-exercicios-obrigatorios).

```python
def main() -> None:
    nome, idade, cidade, estudando = "Ada", 36, "Londres", True
    print(f"nome={nome} idade={idade} cidade={cidade} estudando={estudando}")

    valores = [7, 2.5, True, "texto", None]
    print([type(v).__name__ for v in valores])
    assert [type(v).__name__ for v in valores] == ["int", "float", "bool", "str", "NoneType"]

    x = 10
    print("antes:", x, type(x).__name__)
    x = "dez"  # rebinding: o nome passa a apontar para outro objeto
    print("depois:", x, type(x).__name__)
    assert x == "dez"

    # Cinco expressoes do exercicio 4, conferidas:
    assert 2 + 3 * 4 == 14
    assert 7 / 2 == 3.5 and 7 // 2 == 3 and 7 % 2 == 1
    assert 2 ** 10 == 1024
    print("testes ok")


if __name__ == "__main__":
    main()
```

3. Rebinding nao e mutacao: `x = "dez"` nao altera o inteiro 10 (inteiros sao imutaveis); o **nome** passa a apontar para outro objeto. Essa distincao e a [aula 03](03-solucoes.md) inteira.

**Autoverificacao:** tipos listados na ordem, rebinding demonstrado e as cinco expressoes conferidas com `assert`.
