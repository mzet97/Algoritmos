---
layout: default
---

# Aula 27 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../27.md#13-exercicios-obrigatorios).

```python
import numpy as np


def main() -> None:
    matriz = np.arange(16, dtype=np.int64).reshape(4, 4)
    print(matriz)
    print("shape:", matriz.shape, "| strides (bytes por passo):", matriz.strides,
          "| itemsize:", matriz.itemsize)
    # Andar uma linha = pular 4 itens de 8 bytes; uma coluna = 1 item.
    assert matriz.strides == (32, 8)

    diagonal = matriz[::2, ::2]  # view: linhas e colunas de 2 em 2
    principal = np.diag(matriz)
    print("diagonal via strides:", principal, "| view espaçada:")
    print(diagonal)
    assert principal.tolist() == [0, 5, 10, 15]
    assert np.shares_memory(diagonal, matriz)  # view nao copiou

    copia = matriz.copy()
    assert not np.shares_memory(copia, matriz)

    elementos = 100000
    i32 = np.empty(elementos, dtype=np.int32)
    f64 = np.empty(elementos, dtype=np.float64)
    print("100 mil elementos: int32 =", i32.nbytes, "bytes | float64 =", f64.nbytes, "bytes")
    assert f64.nbytes == 2 * i32.nbytes
    print("testes ok")


if __name__ == "__main__":
    main()
```

1. `strides` sao, em bytes, quanto a memoria anda quando o indice de uma dimensao anda um: `(32, 8)` nesta matriz diz que descer uma linha pula 32 bytes (4 itens de 8) e andar uma coluna pula 8. E assim que a mesma memoria vira matriz, transposta ou view espacada sem copiar nada — a view da diagonal parcial acima e so uma leitura com passos diferentes sobre o mesmo buffer.

5. Vetorizacao reduz overhead do interpretador porque o laco de milhoes de elementos roda em codigo compilado sobre memoria contigua (a [aula 18](18-solucoes.md) mediu), em vez de despachar bytecode, checar tipo e embalar objeto por elemento.

**Autoverificacao:** strides conferidos byte a byte, view compartilhando memoria contra copia nao compartilhando e o dobro de bytes do `float64` sobre o `int32`.
