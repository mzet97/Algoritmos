---
layout: default
---

# Aula 29a — Multicore, HPC e modelo de GPU sem placa

## Objetivo da Aula

Medir onde o paralelismo ajuda Python, onde atrapalha e quando uma GPU valeria a pena — tudo com CPU comum, referência escalar verificada e a conta da transferência explícita. Pré-requisitos: aulas 14a, 16a, 17, 18 e 29. Não precisa de placa GPU, CUDA, MPI nem cluster.

Ao terminar, você deverá comparar referência escalar, NumPy, threads e processos na mesma carga com o mesmo resultado, explicar cada diferença pelo GIL, pelo IPC ou pela vetorização — e decidir no papel se um acelerador pagaria a viagem.

Esta aula é o núcleo obrigatório do módulo PY8: referência em CPU executável, modelo de execução, transferência host↔dispositivo, granularidade e critério de ganho. Placa física mora no laboratório opcional.

## 1. O mapa do paralelismo em Python

- **Laço puro:** referência legível, lenta. É o baseline correto — e o único que você entende linha a linha.
- **NumPy:** vetoriza em C e *libera o GIL* durante o kernel. Uma chamada NumPy já é "multicore-friendly" sem threads suas.
- **Threads + NumPy:** como o GIL está liberado dentro da operação nativa, threads *podem* paralelizar de verdade. Com laço puro, não.
- **Processos:** escapam do GIL para qualquer código, mas pagam spawn + serialização (IPC). Só compensam acima de certo tamanho.
- **GPU (modelo):** mesma conta da aula C 30a — `t_total = t_ida + t_kernel + t_volta`. Sem a viagem, o número é fantasia.

## 2. Hipótese antes do cronômetro

Para soma de quadrados de 1 milhão de inteiros, a previsão honesta é: NumPy muito mais rápido que laço (C contra interpretador); threads sobre NumPy empatam ou ganham pouco (o trabalho já é rápido, o fatiamento custa); processos perdem (IPC de megabytes para milissegundos de conta). Se a medição contradisser, a hipótese escrita é o que torna o resultado interessante em vez de confuso.

## 3. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/29a_hpc.py) contém exatamente o programa abaixo. Precisa de NumPy (`pip install numpy`); o resto é biblioteca padrão:

```bash
python3 teoria/python/exemplos/29a_hpc.py
```

```python
"""Baseline escalar, NumPy, threads, processos e a conta da GPU."""

import os
import time
from concurrent import futures

try:
    import numpy as np
except ImportError:  # pragma: no cover - ambiente sem numpy
    raise SystemExit("instale numpy: pip install numpy")


def referencia(xs):
    """Baseline legivel: soma de quadrados, O(n) com constante alta."""
    total = 0
    for x in xs:
        total += x * x
    return total


def medir(funcao, *args, repeticoes=5):
    melhor = float("inf")
    resultado = None
    for _ in range(repeticoes):
        inicio = time.perf_counter()
        resultado = funcao(*args)
        dur = time.perf_counter() - inicio
        if dur < melhor:
            melhor = dur
    return resultado, melhor


def soma_numpy(bloco):
    return int((bloco * bloco).sum())


def soma_fatia(par):
    a, b = par
    return sum(i * i for i in range(a, b))


def main():
    n = 1_000_000
    nbytes = n * 8
    print(f"n={n} bytes={nbytes} cpus={os.cpu_count()}")

    xs = list(range(n))
    arr = np.arange(n, dtype="int64")

    esperado, t_ref = medir(referencia, xs, repeticoes=3)
    print(f"referencia: {t_ref * 1e3:.1f} ms")

    r_np, t_np = medir(soma_numpy, arr, repeticoes=5)
    assert r_np == esperado, (r_np, esperado)
    print(f"numpy: {t_np * 1e3:.3f} ms (x{t_ref / t_np:.0f} contra referencia)")

    fatias = [(i * n // 4, (i + 1) * n // 4) for i in range(4)]
    partes = [(arr[a:b]) for a, b in fatias]
    with futures.ThreadPoolExecutor(max_workers=4) as exe:
        def roda_threads():
            return sum(exe.map(soma_numpy, partes))
        r_thr, t_thr = medir(roda_threads, repeticoes=3)
    assert r_thr == esperado, r_thr
    print(f"threads+numpy: {t_thr * 1e3:.3f} ms (x{t_ref / t_thr:.0f})")

    with futures.ProcessPoolExecutor(max_workers=4) as exe:
        def roda_processos():
            return sum(exe.map(soma_fatia, fatias))
        t0 = time.perf_counter()
        r_pro = roda_processos()
        t_pro = time.perf_counter() - t0
    assert r_pro == esperado, r_pro
    print(f"processos: {t_pro * 1e3:.1f} ms (mesmo resultado, paga spawn+IPC)")

    # Conta da GPU: aritmetica deterministica, sem hardware.
    banda_gbs = 16.0
    t_ida = (nbytes / 1024**3) / banda_gbs
    # Ida e volta com o mesmo volume, kernel hipotetico de 1 ms:
    t_kernel = 0.001
    t_total = 2 * t_ida + t_kernel
    print(f"viagem: ida+volta={2 * t_ida * 1e3:.2f} ms kernel=1.00 ms total={t_total * 1e3:.2f} ms")
    print(f"numpy local: {t_np * 1e3:.3f} ms")
    if t_total < t_np:
        print("veredito: a GPU hipotetica venceria NESTE cenario")
    else:
        print("veredito: nem kernel instantaneo compensaria sem banda maior; fique na CPU")
    assert t_ida > 0
    print("modelo ok: referencia, multicore medido e conta da transferencia")


if __name__ == "__main__":
    raise SystemExit(main())
```

Saída esperada (tempos variam por máquina, carga e versão do NumPy):

```text
n=1000000 bytes=8000000 cpus=N
referencia: ~ms
numpy: ~ms (x~ contra referencia)
threads+numpy: ~ms (x~)
processos: ~ms (mesmo resultado, paga spawn+IPC)
viagem: ida+volta=~ms kernel=1.00 ms total=~ms
numpy local: ~ms
veredito: ...
modelo ok: referencia, multicore medido e conta da transferencia
```

Leia com o modelo certo: a referência existe para ser *certa*, não rápida; o NumPy vence por rodar em C sem GIL; threads sobre NumPy empatam ou ganham pouco porque o trabalho já é barato; processos pagam spawn+IPC e só fazem sentido bem acima desta carga. O veredito da GPU sai da aritmética, não do marketing: com 8 MB e 16 GiB/s, a viagem custa ~1 ms — contra kernel de 1 ms e NumPy local de ~1 ms, a resposta honesta costuma ser "fique na CPU".

## 4. Erros comuns e implicações defensivas

1. **Medir sem referência verificada:** `assert` de igualdade contra a baseline em *todas* as variantes. Benchmark errado mede a velocidade do erro.
2. **Threads com laço puro e promessa de ganho:** o GIL serializa bytecode. Threads só paralelizam o que libera o GIL (NumPy, I/O) ou espera.
3. **Processos sem contar o IPC:** serializar megabytes para economizar milissegundos é prejuízo documentável — meça o ponto de virada.
4. **Mínimo de uma rodada só:** use o mínimo de várias; anote ambiente, versão do NumPy e carga.
5. **Alegar GPU sem a viagem:** some ida e volta *com a banda medida ou declarada*. Sem isso, o número é fantasia.
6. **Comparar precisão sem checar tipo:** `int64` estoura em `N` grande; confira dtype e overflow antes de comemorar dígitos.

## 5. Ponte com as demais trilhas

C mede o mesmo tradeoff entre baseline e multicore com pthreads (aula 28a); Go mede worker pool e race (aula 11a); Rust compara escalar contra multicore antes de qualquer `wgpu` (aula R7 futura). O método — baseline, hipótese, mínimo de repetições, ambiente anotado — é o mesmo em todas.

## 6. Exercícios obrigatórios

1. Suba `n` para 10 milhões e refaça a tabela. O que muda na razão threads/processos? Explique com IPC e banda.
2. Troque o kernel por `sqrt` elemento a elemento (`np.sqrt` contra laço com `math.sqrt`). A razão NumPy muda? Por quê?
3. Ache o menor `n` em que processos empatam com NumPy puro na sua máquina. Anote ambiente e números.
4. Refaça a conta da GPU com banda de 100 GiB/s (NVLink didático) e kernel de 10 ms. O veredito muda? Mostre a conta.

Use [pistas graduais](exercicios/29a_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/29a_solucoes.md) depois de registrar sua própria explicação.

## 7. Critério de aprovação e checklist

Você concluiu a aula se consegue, sem olhar a solução:

- explicar por que threads ajudam NumPy e não laço puro;
- medir as quatro variantes com igualdade verificada e interpretar cada razão;
- calcular o ponto de equilíbrio de processos contra NumPy;
- fazer a conta da viagem com banda declarada e emitir o veredito com os números;
- apontar o que o experimento prova e o que continua sem hardware para provar.

**Próxima aula:** backend didático. O raciocínio desta aula sustenta a escolha de onde cada milissegundo mora antes de escalar o serviço.
