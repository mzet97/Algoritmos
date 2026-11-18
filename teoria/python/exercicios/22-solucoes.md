---
layout: default
---

# Aula 22 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../22.md#13-exercicios-obrigatorios).

```python
import time
from dataclasses import dataclass, field


@dataclass
class Job:
    tipo: str
    dados: dict
    tentativas: int = 0


def gerar_relatorio(job: Job) -> str:
    total = sum(job.dados["valores"])
    return f"relatorio de {job.dados['nome']}: total={total}"


METRICAS = {"ok": 0, "retentaveis": 0, "invalidos": 0}


def processar(job: Job, esperas: list[float], falhar_ate: int = 0) -> str:
    if job.tipo != "relatorio" or "valores" not in job.dados:
        METRICAS["invalidos"] += 1
        return "invalido"  # fila conceitual separada: sem retry (exercicio 2)
    while True:
        job.tentativas += 1
        if job.tentativas <= falhar_ate:
            METRICAS["retentaveis"] += 1
            espera = min(0.01 * (2 ** (job.tentativas - 1)), 0.08)
            esperas.append(espera)
            time.sleep(espera)
            continue
        METRICAS["ok"] += 1
        return gerar_relatorio(job)


def main() -> None:
    esperas: list[float] = []
    resultado = processar(Job("relatorio", {"nome": "vendas", "valores": [10, 20, 12]}),
                          esperas, falhar_ate=2)
    print(resultado, "| esperas de backoff:", esperas)
    assert resultado.endswith("total=42") and esperas == [0.01, 0.02]

    destino = processar(Job("email", {"para": "ada@exemplo.dev"}), [])
    print("job de outro tipo/invalido vai para a fila separada:", destino)
    assert destino == "invalido"
    print("metricas:", dict(METRICAS))
    assert METRICAS == {"ok": 1, "retentaveis": 2, "invalidos": 1}
    print("testes ok")


if __name__ == "__main__":
    main()
```

1. As esperas medidas sao 0.01, 0.02 (a proxima seria 0.04, com teto 0.08): crescente com teto, porque backoff sem teto vira espera infinita disfarcada, e sem crescimento vira martelada sincronizada na dependencia que ja esta mal.

3. Retry sem idempotencia e perigoso quando o efeito nao pode se repetir: o email que ja saiu, o pagamento que ja debitou, o `INSERT` sem chave unica da [aula 21](21-solucoes.md). A pergunta antes de qualquer retry e "o que acontece se isto executar duas vezes?" — se a resposta for dano, ou o job ganha chave de idempotencia, ou nao ganha retry.

5. Metricas minimas do worker: jobs processados por desfecho (ok/retentavel/invalido), tentativas por job, tempo por job e tamanho da fila. Sao os contadores acima com nome e etiqueta — o resto e painel.

**Autoverificacao:** relatorio no lugar do email, backoff medido e crescente, invalido sem retry e metricas por desfecho.
