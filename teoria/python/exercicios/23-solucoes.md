---
layout: default
---

# Aula 23 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../23.md#13-exercicios-obrigatorios).

```python
import logging
import uuid
from contextlib import contextmanager

logging.basicConfig(level=logging.INFO, format="%(message)s")
log = logging.getLogger("aula23")

ERROS: dict[str, int] = {}
SPANS: list[str] = []
TOTAL = {"requisicoes": 0, "sucessos": 0}


@contextmanager
def span(nome: str):
    SPANS.append(nome + ":inicio")
    try:
        yield
    finally:
        SPANS.append(nome + ":fim")


def atender(request_id: str, falhar: bool) -> bool:
    TOTAL["requisicoes"] += 1
    try:
        with span("banco"):
            if falhar:
                raise TimeoutError("banco lento")
        TOTAL["sucessos"] += 1
        log.info("request_id=%s resultado=ok", request_id)
        return True
    except TimeoutError as e:
        ERROS[type(e).__name__] = ERROS.get(type(e).__name__, 0) + 1
        log.info("request_id=%s resultado=erro tipo=%s", request_id, type(e).__name__)
        return False


def main() -> None:
    ids = [uuid.uuid4().hex[:8] for _ in range(3)]
    resultados = [atender(ids[0], False), atender(ids[1], True), atender(ids[2], False)]
    taxa = TOTAL["sucessos"] / TOTAL["requisicoes"]
    print("resultados:", resultados, "| sucesso por request:", round(taxa, 2))
    print("erros por tipo:", ERROS, "| spans:", SPANS)
    assert resultados == [True, False, True] and ERROS == {"TimeoutError": 1}
    assert SPANS == ["banco:inicio", "banco:fim"] * 3
    print("testes ok")


if __name__ == "__main__":
    main()
```

3. `request_id` importa porque o log de uma requisicao se espalha por servicos e horas: sem o fio, voce tem mil linhas de "erro no banco" e nenhuma maneira de saber quais pertencem ao pedido da Ada. Com ele, o incidente vira uma consulta, nao uma escavacao.

4. Tres spans uteis numa API real: a chamada ao banco (ou ao trecho SQL caro), a chamada HTTP a servico externo e a publicacao/consumo na fila. Sao as fronteiras onde o tempo some e onde outro dono comeca.

5. Logs sozinhos sao insuficientes para "qual servico da cadeia deixou a requisicao lenta": cada servico loga o seu pedaco sem relacao de ordem nem duracao comparavel entre eles. O trace existe para costurar spans comecando e terminando em relogio comum — log responde "o que", trace responde "onde no caminho".

**Autoverificacao:** erro contado por tipo, taxa de sucesso calculada, spans abrindo e fechando por requisicao e todo log carregando `request_id`.
