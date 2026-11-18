---
layout: default
---

# Aula 26 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../26.md#13-exercicios-obrigatorios).

```python
import json
import logging
import tempfile
from datetime import datetime, timezone
from pathlib import Path

logging.basicConfig(level=logging.INFO, format="%(message)s")
log = logging.getLogger("aula26")


def processar(entrada: Path, saida: Path, checkpoint: Path, reset: bool = False) -> int:
    if reset and checkpoint.exists():
        checkpoint.unlink()
    ja_feitas = 0
    if checkpoint.exists():
        estado = json.loads(checkpoint.read_text(encoding="utf-8"))
        ja_feitas = int(estado["ultima_linha"])
    linhas = entrada.read_text(encoding="utf-8").splitlines()
    log.info("job inicio total=%d retomando_de=%d reset=%s", len(linhas), ja_feitas, reset)
    processadas = 0
    with saida.open("a", encoding="utf-8") as destino:
        for numero, linha in enumerate(linhas, start=1):
            if numero <= ja_feitas:
                continue
            destino.write(linha.upper() + chr(10))
            processadas += 1
            estado = {"ultima_linha": numero,
                      "quando": datetime.now(timezone.utc).isoformat()}
            checkpoint.write_text(json.dumps(estado), encoding="utf-8")
    log.info("job fim processadas=%d", processadas)
    return processadas


def main() -> None:
    with tempfile.TemporaryDirectory() as tmp:
        raiz = Path(tmp)
        entrada = raiz / "entrada.txt"
        entrada.write_text(chr(10).join(["um", "dois", "tres", "quatro"]) + chr(10), encoding="utf-8")
        saida, checkpoint = raiz / "saida.txt", raiz / "checkpoint.json"

        primeira = processar(entrada, saida, checkpoint)
        segunda = processar(entrada, saida, checkpoint)  # retomada: nada a fazer
        estado = json.loads(checkpoint.read_text(encoding="utf-8"))
        print(f"1a={primeira} retomada={segunda} checkpoint={estado}")
        assert (primeira, segunda) == (4, 0) and estado["ultima_linha"] == 4 and "quando" in estado
        assert saida.read_text(encoding="utf-8").splitlines() == ["UM", "DOIS", "TRES", "QUATRO"]

        saida.unlink()
        terceira = processar(entrada, saida, checkpoint, reset=True)
        print("apos reset, reprocessou tudo:", terceira)
        assert terceira == 4
    print("testes ok")


if __name__ == "__main__":
    main()
```

4. Reexecucao segura importa porque ETL cai no meio em producao, nao no laboratorio: sem checkpoint, a escolha e reprocessar duplicando a saida ou perder o restante. Com checkpoint por linha e saida idempotente, rodar de novo e a operacao normal de recuperacao — o mesmo desenho do checkpoint dos [projetos](../projetos/README.md) desta trilha.

5. Metricas uteis do job: linhas lidas/escritas/ignoradas por retomada, tempo total, instante do ultimo checkpoint e causa da parada. Sem a ultima, o alarme das 3h nao distingue "terminou" de "morreu na linha 900 mil".

**Autoverificacao:** primeira passada completa, retomada processando zero, checkpoint com timestamp e reset reprocessando tudo sem duplicar a saida.
