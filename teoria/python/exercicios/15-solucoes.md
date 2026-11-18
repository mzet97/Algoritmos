---
layout: default
---

# Aula 15 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../15.md#20-exercicios-obrigatorios).

```python
import threading
import time
from multiprocessing import Pool


def quadrado(n: int) -> int:
    return n * n


def main() -> None:
    inicio = time.perf_counter()
    threads = [threading.Thread(target=time.sleep, args=(0.2,)) for _ in range(2)]
    for t in threads:
        t.start()
    for t in threads:
        t.join()
    duracao = time.perf_counter() - inicio
    print(f"duas esperas de 0.2s em threads: {duracao:.2f}s (juntas, nao em fila)")
    assert duracao < 0.35

    contador = 0
    trava = threading.Lock()

    def incrementar() -> None:
        nonlocal contador
        for _ in range(10000):
            with trava:
                contador += 1

    trabalhadores = [threading.Thread(target=incrementar) for _ in range(4)]
    for t in trabalhadores:
        t.start()
    for t in trabalhadores:
        t.join()
    print("contador com Lock:", contador)
    assert contador == 40000

    with Pool(processes=2) as pool:
        resultados = pool.map(quadrado, [1, 2, 3, 4])
    assert resultados == [1, 4, 9, 16]
    print("Pool.map:", resultados)
    print("testes ok")


if __name__ == "__main__":
    main()
```

5. Classificacao de cinco workloads: baixar arquivos (I/O), esperar resposta de API (I/O), redimensionar imagens em lote (CPU), treinar modelo (CPU), consultar banco repetidamente (I/O, ate o banco virar o gargalo). A pergunta e sempre "o tempo vai para **espera** ou para **queima**?" — threads servem a espera; processos, a queima, como mediu a [ponte 16a](16a_solucoes.md).

**Autoverificacao:** esperas sobrepostas no relogio, 40000 exatos com `Lock` e `Pool.map` na ordem da entrada.
