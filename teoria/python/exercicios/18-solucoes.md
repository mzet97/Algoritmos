---
layout: default
---

# Aula 18 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../18.md#18-exercicios-obrigatorios).

```python
import time

import numpy as np


def main() -> None:
    valores = list(range(100000))
    inicio = time.perf_counter()
    soma_loop = sum(v * 2 for v in valores)
    t_loop = time.perf_counter() - inicio

    arr = np.arange(100000, dtype=np.int64)
    inicio = time.perf_counter()
    soma_np = int((arr * 2).sum())
    t_np = time.perf_counter() - inicio
    print(f"soma loop={soma_loop} ({t_loop * 1000:.1f}ms) numpy={soma_np} ({t_np * 1000:.1f}ms)")
    assert soma_loop == soma_np

    a = np.array([1, 2, 3])
    b = np.array([10, 20, 30])
    grade = np.array([[1], [2]]) + np.array([10, 20, 30])  # broadcasting (2,1)+(3,)
    print("soma:", a + b, "| multiplicacao:", a * b, "| broadcasting:")
    print(grade)
    assert (a + b).tolist() == [11, 22, 33] and (a * b).tolist() == [10, 40, 90]
    assert grade.tolist() == [[11, 21, 31], [12, 22, 32]]
    print("testes ok")


if __name__ == "__main__":
    main()
```

3. Caminhos de integracao com C/C++ para um hotspot, do mais leve ao mais profundo: chamar biblioteca compilada existente (NumPy e familia), `ctypes`/`cffi` para funcao pronta, extensao C/C++ ou Cython para codigo novo, Numba para laco numerico anotado. A escolha comeca medindo onde o tempo vai ([aula 17](17-solucoes.md)), nao pela ferramenta favorita.

4. Python como orquestracao: num pipeline de imagens, o Python le config, enfileira e registra, enquanto decode/redimensionamento rodam em biblioteca compilada. O criterio: se o laco quente esta em Python puro sobre milhoes de itens, ele e o candidato a descer; todo o resto fica.

5. Lista Python guarda ponteiros para objetos heterogeneos espalhados; array NumPy guarda valores do mesmo `dtype` lado a lado. Contiguidade e tipo unico sao o que permitem a operacao vetorizada rodar sem interpretador por elemento.

**Autoverificacao:** mesma soma pelos dois caminhos, broadcasting com forma conferida e a distincao lista/array enunciada com a causa.
