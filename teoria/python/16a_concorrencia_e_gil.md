---
layout: default
---

# Aula 16a - Concorrência, GIL e paralelismo real em Python

## Nesta aula

Distinguir o que `threading`, `multiprocessing` e `asyncio` garantem — e o que não garantem. Pré-requisitos: aulas 14a, 15 e 16. Não precisa saber C, Rust, rede nem `cgo`.

Você vai aprender a explicar o GIL e quando há paralelismo real, comparar threads/processos/`async`, e medir para decidir.

## 1. O GIL, sem mito e sem desculpa

O GIL (Global Interpreter Lock) do CPython permite que apenas uma thread execute bytecode por vez. Consequências diretas, por versão:

- **I/O-bound com threads:** funciona — threads liberam o GIL enquanto esperam I/O.
- **CPU-bound com threads:** não paraleliza em CPython com GIL (até 3.12, o cenário deste livro); pode até piorar pelo custo de troca.
- **CPU-bound de verdade:** `multiprocessing` (isolamento por processo + IPC) ou código nativo que libera o GIL (NumPy, algumas extensões).

A partir do 3.13 existe build *free-threaded* experimental sem GIL — mas muda o contrato de thread-safety de extensões e não é o alvo deste livro. Não alegue propriedade do GIL sem dizer a versão: `sys.version` e `sys._is_gil_enabled()` (3.13+) decidem, não a memória.

## 2. Corrida demonstrada, não alegada

`x += 1` em Python são três passos (ler, somar, escrever) — mas sem chamada no meio, a sequência curta raramente é interrompida pelo GIL, e a demonstração "ingênua" pode passar intacta. Por isso o exemplo separa leitura e escrita com um `time.sleep(0)` no meio: o ponto de troca explícito faz outra thread entrar entre ler e escrever, e a perda vira regra (~20 mil em vez de 160 mil). "Quase sempre" continua não sendo contrato: o exemplo *mede* o caso sem lock (ilustrativo) e *afirma* apenas o caso com `Lock` (800.000 exatos). Demonstrar a corrida educa; provar a correção exige o invariante "todo acesso sob lock".

Detalhe que virou lição: a função enviada a processos precisa ser picklable — `lambda` e closures locais não atravessam a fronteira do processo. `soma_fatia` vive no nível do módulo exatamente por isso; o erro `Can't pickle local object` é o SO cobrando o contrato de serialização.

## 3. O cardápio honesto

| Carga | Ferramenta | Por quê |
| --- | --- | --- |
| I/O concorrente, milhares de conexões | `asyncio` | um loop, sem threads, sem GIL no caminho |
| I/O com biblioteca bloqueante | `threading` | threads liberam o GIL no I/O |
| CPU-bound puro | `multiprocessing` / nativo | processos escapam do GIL; paga-se IPC e spawn |
| Orquestrar as anteriores | `concurrent.futures` | mesma interface, executores trocáveis |

Medir o custo de IPC e de spawn faz parte do contrato (o mapa PY5 exige): processo que nasce para somar 1 ms de conta é prejuízo documentável, não aceleração.

## 4. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/16a_concorrencia.py) contém exatamente o programa abaixo. Só biblioteca padrão:

```bash
python3 teoria/python/exemplos/16a_concorrencia.py
```

```python
"""Concorrencia em Python: corrida demonstrada, Lock provado, medidas honestas."""

import asyncio
import sys
import threading
import time
from concurrent import futures


def soma_fatia(par: tuple) -> int:
    """Soma i*i no intervalo [a, b). Nivel de modulo: picklable para processos."""
    a, b = par
    return sum(i * i for i in range(a, b))


def corrida_sem_lock(threads: int = 8, repeticoes: int = 100_000) -> int:
    total = 0

    def incrementar() -> None:
        nonlocal total
        for _ in range(repeticoes):
            atual = total
            # sleep(0) cede o GIL ENTRE a leitura e a escrita: outra
            # thread entra no meio e escreve por cima. Sem esse ponto de
            # troca, a sequencia curta raramente e interrompida.
            time.sleep(0)
            total = atual + 1  # escreve sobre valor velho: perde atualizacoes

    ts = [threading.Thread(target=incrementar) for _ in range(threads)]
    for t in ts:
        t.start()
    for t in ts:
        t.join()
    return total


def corrida_com_lock(threads: int = 8, repeticoes: int = 100_000) -> int:
    total = 0
    trava = threading.Lock()

    def incrementar() -> None:
        nonlocal total
        for _ in range(repeticoes):
            with trava:
                total += 1

    ts = [threading.Thread(target=incrementar) for _ in range(threads)]
    for t in ts:
        t.start()
    for t in ts:
        t.join()
    return total


def medir(funcao, *args):
    inicio = time.perf_counter()
    resultado = funcao(*args)
    return resultado, time.perf_counter() - inicio


async def tarefa_io(nome: str, espera: float) -> str:
    await asyncio.sleep(espera)  # cooperativo: nao bloqueia o loop
    return nome


async def demo_asyncio() -> list:
    return await asyncio.gather(
        *(tarefa_io(f"t{i}", 0.05) for i in range(20)),
    )


def main() -> int:
    print(f"versao={sys.version.split()[0]} gil={getattr(sys, '_is_gil_enabled', lambda: 'n/a (<=3.12)')()}")
    esperado = 8 * 100_000

    sem_lock, t_sem = medir(corrida_sem_lock, 8, 20_000)
    print(f"sem lock: total={sem_lock} (esperado {8 * 20_000}) em {t_sem:.2f}s -- ilustrativo, nao contrato")
    com_lock, t_lock = medir(corrida_com_lock)
    assert com_lock == esperado, com_lock
    print(f"com lock: total={com_lock} em {t_lock:.2f}s -- contrato cumprido")

    # CPU-bound: sequencial x threads x processos (mesmo resultado, custos diferentes).
    n, partes = 1_000_000, 4
    fatias = [(i * n // partes, (i + 1) * n // partes) for i in range(partes)]
    esperado_soma = sum(i * i for i in range(n))

    t0 = time.perf_counter()
    seq = sum(soma_fatia(ab) for ab in fatias)
    t_seq = time.perf_counter() - t0

    with futures.ThreadPoolExecutor(max_workers=4) as exe:
        t0 = time.perf_counter()
        par_t = sum(exe.map(soma_fatia, fatias))
        t_thr = time.perf_counter() - t0

    with futures.ProcessPoolExecutor(max_workers=4) as exe:
        t0 = time.perf_counter()
        par_p = sum(exe.map(soma_fatia, fatias))
        t_pro = time.perf_counter() - t0

    assert seq == par_t == par_p == esperado_soma, (seq, par_t, par_p)
    print(f"cpu seq={t_seq:.3f}s threads={t_thr:.3f}s processos={t_pro:.3f}s (mesmo resultado)")

    # Asyncio: 20 tarefas de 50 ms em ~50 ms totais, mais timeout que funciona.
    t0 = time.perf_counter()
    nomes = asyncio.run(demo_asyncio())
    t_async = time.perf_counter() - t0
    assert sorted(nomes) == sorted(f"t{i}" for i in range(20)), nomes
    print(f"asyncio: 20 tarefas em {t_async * 1e3:.1f} ms")

    try:
        asyncio.run(asyncio.wait_for(asyncio.sleep(5), timeout=0.1))
        print("INESPERADO: timeout nao disparou")
        return 2
    except TimeoutError:
        print("timeout contido: TimeoutError em 0.1s")

    # Processo filho com codigo de saida e sem orfaos.
    import subprocess
    p = subprocess.run([sys.executable, "--version"], capture_output=True, text=True, timeout=10)
    assert p.returncode == 0, p.stderr
    print("filho:", (p.stdout or p.stderr).strip())
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
```

Saída esperada (tempos variam por máquina, carga e versão):

```text
versao=3.12.3 gil=...
sem lock: total=~20000 (esperado 160000) em ~s -- ilustrativo, nao contrato
com lock: total=800000 em ~s -- contrato cumprido
cpu seq=~s threads=~s processos=~s (mesmo resultado)
asyncio: 20 tarefas em ~50 ms
timeout contido: TimeoutError em 0.1s
filho: Python 3.12.3
```

Leia com o modelo certo: threads empatam ou perdem no CPU-bound (GIL) e vencem no I/O; processos pagam spawn+IPC e só compensam em carga grande; `asyncio` comprime 20 esperas de 50 ms em ~50 ms porque esperar não é trabalhar. Nenhum número promete nada sem ambiente anotado.

### O que a memória faz aqui?

Threads dividem o mesmo heap: é por isso que `total += 1` corre — e por isso o `Lock` resolve. Processos não dividem nada: cada um tem heap próprio e o resultado viaja serializado pelo IPC. `asyncio` roda tudo numa thread só: sem corrida de memória entre corrotinas, mas com as mesmas corridas lógicas de ordem de conclusão.

## 5. Erros comuns e implicações defensivas

1. **Tratar GIL como barreira de segurança:** não é. Protege o interpretador, não seus dados.
2. **Afirmar o caso sem lock:** corrida demonstrada ilustra; contrato exige lock + assert exato.
3. **Medir processos em carga pequena:** spawn+IPC dominam. Meça o ponto de equilíbrio, não o microcaso.
4. **Esquecer `join`/shutdown:** thread não finalizada e executor sem contexto viram vazamento e órfãos. Use `with` nos executores.
5. **Timeout sem cancelamento real:** `wait_for` cancela a tarefa; timeout manual com flag pode deixar trabalho rodando. Prefira o mecanismo que cancela.
6. **Prometer speedup com mais workers:** contenção, GIL e IPC decidem. Meça em dois tamanhos antes de alegar.

## 6. Ponte com Go e JavaScript

Go executa goroutines em paralelo real e detecta data race com `-race`; Python detecta com teste + `Lock`, sem ferramenta equivalente embutida. Node serializa callbacks num loop como o `asyncio`. Nos três, a disciplina é igual: dono dos dados, fechamento único, cancelamento cooperativo, timeout que cancela de verdade.

## 7. Exercícios obrigatórios

1. Reescreva por que `x += 1` perde atualizações sem lock, em termos de ler-somar-escrever.
2. Varie threads (1, 2, 4, 8) no caso sem lock com carga fixa e registre a dispersão em 5 rodadas. Explique por que o número dança.
3. Ache o tamanho de carga em que processos empatam com threads no CPU-bound da sua máquina. Anote ambiente e números.
4. Adicione cancelamento cooperativo a um pool de threads com `threading.Event` e prove que todas terminam.

Use [pistas graduais](exercicios/16a_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/16a_solucoes.md) depois de registrar sua própria explicação.

## 8. Ao final, você consegue

Você concluiu a aula se consegue, sem olhar a solução:

- explicar o GIL por versão e quando ele ajuda, quando atrapalha e quando é irrelevante;
- demonstrar uma corrida e provar a correção com lock + assert exato;
- medir threads contra processos na mesma carga e explicar cada número;
- usar `asyncio` para I/O concorrente e `wait_for` para timeout real;
- executar o exemplo e confrontar cada número com o modelo;
- apontar o que cada ferramenta garante e o que ela não garante.

**Próxima aula:** performance tuning e profiling. As réguas desta aula (tempo monotônico, ambiente anotado, dois tamanhos) viram método de benchmark.

---

### Como esta aula será avaliada

| Critério | Peso |
| -------- | ---: |
| Conceitos e vocabulário (nível 1) | 20% |
| Previsão de comportamento e diagnóstico de erros (nível 2) | 40% |
| Projeto e justificativa de decisões (nível 3) | 30% |
| Registro de comandos, saídas e interpretação | 10% |

**Aprovação:** nota mínima 7,0; pelo menos 60% dos pontos devem vir de previsão e diagnóstico (níveis 2 e 3), não apenas de código que executa.

**Para se aprofundar:**

1. *High Performance Python*, Gorelick e Ozsvald (O'Reilly, ISBN 978-1492055020); 2. Documentação de `threading`/`asyncio` em [docs.python.org/3/library](https://docs.python.org/3/library/); 3. Luciano Ramalho, *Python Fluente*, caps. sobre concorrência.
