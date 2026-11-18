---
layout: default
---

# Aula 25 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../25.md#13-exercicios-obrigatorios). O alvo do wrapper, neste laboratorio, e um processo local que imita o comportamento do `ping` (responde, falha, demora) sem depender da rede da maquina:

```python
import logging
import subprocess
import sys
import time

logging.basicConfig(level=logging.INFO, format="%(message)s")
log = logging.getLogger("aula25")


def classificar(resultado: subprocess.CompletedProcess | None, estourou: bool) -> str:
    if estourou:
        return "retryable"       # timeout pode ser transitorio
    if resultado is not None and resultado.returncode == 0:
        return "ok"
    return "nao-retryable"       # destino/falha dura: repetir igual nao muda


def executar(comando: list[str], timeout: float) -> tuple[str, subprocess.CompletedProcess | None]:
    inicio = time.perf_counter()
    log.info("chamando alvo=%s timeout=%ss", comando[1] if len(comando) > 1 else comando[0], timeout)
    try:
        resultado = subprocess.run(comando, capture_output=True, text=True, timeout=timeout)
        classe = classificar(resultado, False)
        log.info("retorno codigo=%s classe=%s duracao_ms=%.0f",
                 resultado.returncode, classe, (time.perf_counter() - inicio) * 1000)
        return classe, resultado
    except subprocess.TimeoutExpired:
        log.info("timeout classe=retryable duracao_ms=%.0f", (time.perf_counter() - inicio) * 1000)
        return classificar(None, True), None


def alvo(codigo_python: str) -> list[str]:
    return [sys.executable, "-c", codigo_python]


def main() -> None:
    classe_ok, r_ok = executar(alvo("print('pong')"), timeout=2)
    classe_falha, r_falha = executar(alvo("raise SystemExit(2)"), timeout=2)
    classe_lento, _ = executar(alvo("import time; time.sleep(5)"), timeout=0.2)
    print("classes:", classe_ok, classe_falha, classe_lento,
          "| codigos:", r_ok.returncode if r_ok else None, r_falha.returncode if r_falha else None)
    assert (classe_ok, classe_falha, classe_lento) == ("ok", "nao-retryable", "retryable")
    # ssh modelado (exercicio 4): mesma lista, mesmo wrapper, outro alvo —
    # ["ssh", "-o", "ConnectTimeout=3", "host", "comando"] com timeout total no run.
    print("testes ok")


if __name__ == "__main__":
    main()
```

3. Lista de argumentos e mais segura que string de shell porque cada argumento chega ao processo como **um** argumento: um nome de arquivo com espacos ou `;` nao vira comando novo, porque nenhum shell interpreta a linha. String crua entrega o texto ao shell — e o shell obedece ao texto inteiro, inclusive a parte que veio do usuario.

5. Logs ao redor de uma integracao HTTP critica: antes, alvo e timeout (sem segredos nem corpo); depois, status/codigo, classificacao e duracao; no timeout, a classe retryable explicitada. E o bastante para distinguir "o vizinho esta lento" de "nosso prazo esta curto" as 3h da manha.

**Autoverificacao:** tres comportamentos do alvo classificados, timeout cortando de verdade e nenhum shell entre voce e o processo.
