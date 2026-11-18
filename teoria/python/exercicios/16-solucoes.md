---
layout: default
---

# Aula 16 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../16.md#19-exercicios-obrigatorios).

```python
import asyncio
import time


async def tarefa(nome: str, espera: float, feitas: list[str]) -> str:
    await asyncio.sleep(espera)
    feitas.append(nome)
    return nome


async def main_async() -> None:
    feitas: list[str] = []
    inicio = time.perf_counter()
    resultados = await asyncio.gather(
        tarefa("a", 0.15, feitas), tarefa("b", 0.15, feitas))
    duracao = time.perf_counter() - inicio
    print(f"gather de duas esperas de 0.15s: {duracao:.2f}s", resultados)
    assert resultados == ["a", "b"] and duracao < 0.28

    try:
        await asyncio.wait_for(tarefa("lenta", 5.0, feitas), timeout=0.05)
    except TimeoutError:
        print("wait_for cortou a lenta pelo relogio")

    trava = asyncio.Lock()
    ordem: list[str] = []

    async def protegido(nome: str) -> None:
        async with trava:
            ordem.append(nome + ":entra")
            await asyncio.sleep(0)
            ordem.append(nome + ":sai")

    await asyncio.gather(protegido("x"), protegido("y"))
    pares = [ordem[i:i + 2] for i in range(0, len(ordem), 2)]
    assert all(p[0].split(":")[0] == p[1].split(":")[0] for p in pares)
    print("trechos protegidos nao se intercalam:", ordem)
    print("testes ok")


def main() -> None:
    asyncio.run(main_async())


if __name__ == "__main__":
    main()
```

3. A diferenca entre os dois `sleep`: `time.sleep(1)` numa coroutine segura o **loop inteiro** por 1s (nenhuma outra coroutine anda); `asyncio.sleep(1)` devolve o controle e o loop serve as outras. E o mesmo contraste que a [ponte 16a](16a_solucoes.md) mediu entre threads, processos e asyncio.

**Autoverificacao:** gather sobreposto no relogio, `wait_for` cortando e trechos com `Lock` sem intercalacao.
