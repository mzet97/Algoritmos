---
layout: default
---

# Soluções comentadas da aula 16a

## 1. Ler-somar-escrever

Bytecode aproximado de `total += 1` sobre variável de closure: `LOAD` (lê), `LOAD_CONST 1`, `BINARY_ADD`, `STORE` (escreve). Se a thread A lê 42 e a thread B lê 42 antes de A escrever 43, B escreve 43 por cima: uma atualização some. O `Lock` serializa o trio; o invariante "todo acesso sob lock" é a prova.

## 2. Dispersão em 5 rodadas

Tipicamente 5 totais diferentes, todos abaixo de 800.000. O escalonador do SO interrompe threads em pontos arbitrários; cada interrupção no meio do trio é uma chance de colisão. Mais threads e mais iterações apenas aumentam a probabilidade — por isso "passou uma vez" nunca foi evidência.

## 3. Ponto de equilíbrio threads x processos

```python
for n in (100_000, 500_000, 1_000_000, 2_000_000):
    ...mesmo protocolo da aula, 4 workers...
```

Em carga pequena os processos perdem feio (spawn + serialização dominam); a partir de certo `n`, o paralelismo real compensa. O ponto exato depende de CPU, SO e versão do Python — publique os três junto com o veredito.

## 4. Cancelamento cooperativo com `Event`

```python
parar = threading.Event()

def worker():
    while not parar.is_set():
        job = fila.get()  # com sentinel None para acordar e sair
        if job is None:
            fila.task_done()
            return
        ... processa ...
        fila.task_done()
```

`parar.set()` + um `None` por worker + `join` com timeout: se algum `join` estourar, o relatório diz qual worker e em que job — prova de término, não esperança.
