---
layout: default
---

# Aula 14 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../14.md#18-exercicios-obrigatorios). Testes escritos como funcoes executaveis neste harness (no seu projeto, o mesmo corpo vira `pytest`):

```python
import logging

logging.basicConfig(level=logging.INFO, format="%(levelname)s %(message)s")
log = logging.getLogger("aula14")


def desconto(preco: float, percentual: float) -> float:
    log.info("desconto preco=%s percentual=%s", preco, percentual)
    if percentual < 0 or percentual > 100:
        raise ValueError("percentual fora de 0..100")
    return preco * (1 - percentual / 100)


def teste_desconto_normal() -> None:
    assert desconto(100.0, 10.0) == 90.0


def teste_desconto_total() -> None:
    assert desconto(50.0, 100.0) == 0.0


def teste_excecao_esperada() -> None:
    try:
        desconto(100.0, 150.0)
    except ValueError:
        return
    raise AssertionError("ValueError nao levantado")


def main() -> None:
    teste_desconto_normal()
    teste_desconto_total()
    teste_excecao_esperada()
    print("3 testes passaram (no projeto: pytest coleta as mesmas funcoes)")
    print("testes ok")


if __name__ == "__main__":
    main()
```

3. O bug proposital para o `breakpoint()`: troque o `1 -` por `1 +` em `desconto`, rode o primeiro teste e, na pausa, inspecione `preco` e `percentual` — o estado vivo mostra o sinal trocado antes de qualquer `print` arqueologico. Reverta depois: bug plantado que fica vira bug de verdade.

5. Mini fluxo local, nesta ordem:

    ruff format . && ruff check . && pytest -q

Formatador primeiro (diff mecanico separado), linter depois (o que a maquina sabe), testes por ultimo (o comportamento).

**Autoverificacao:** dois caminhos de valor, a excecao esperada exigida (o teste falha se ela nao vier) e logs nomeando a decisao.
