---
layout: default
---

# Aula 31 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../31.md#9-exercicios-obrigatorios). Nucleo executado sobre o desenho do [projeto 31](../projetos/31_infra_tool/main.py):

```python
import hashlib
import json
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor
from datetime import datetime, timezone
from pathlib import Path


def sondar(caminho: Path) -> str:
    return hashlib.file_digest(caminho.open("rb"), "sha256").hexdigest()[:12]


def executar(arquivos: list[Path], checkpoint: Path, reset: bool = False,
             concorrencia: int = 2) -> dict[str, str]:
    if reset and checkpoint.exists():
        checkpoint.unlink()
    estado: dict = {"processed": {}, "quando": None}
    if checkpoint.exists():
        estado = json.loads(checkpoint.read_text(encoding="utf-8"))
    pendentes = [a for a in arquivos if a.name not in estado["processed"]]
    with ThreadPoolExecutor(max_workers=concorrencia) as pool:
        for caminho, resumo in zip(pendentes, pool.map(sondar, pendentes)):
            estado["processed"][caminho.name] = resumo
    estado["quando"] = datetime.now(timezone.utc).isoformat()
    checkpoint.write_text(json.dumps(estado), encoding="utf-8")
    print(f"sondadas agora={len(pendentes)} total={len(estado['processed'])} quando={estado['quando']}",
          file=sys.stderr)
    return dict(estado["processed"])


def main() -> None:
    with tempfile.TemporaryDirectory() as tmp:
        raiz = Path(tmp)
        arquivos = []
        for nome, conteudo in (("a.txt", "alfa"), ("b.txt", "beta"), ("c.txt", "gama")):
            caminho = raiz / nome
            caminho.write_text(conteudo, encoding="utf-8")
            arquivos.append(caminho)
        checkpoint = raiz / "checkpoint.json"

        primeira = executar(arquivos, checkpoint)
        segunda = executar(arquivos, checkpoint)          # checkpoint pula todas
        terceira = executar(arquivos, checkpoint, reset=True)  # reset ressonda todas
        print("resumos iguais nas tres passadas:", primeira == segunda == terceira)
        assert primeira == segunda == terceira and len(primeira) == 3
        estado = json.loads(checkpoint.read_text(encoding="utf-8"))
        assert estado["quando"] is not None
    print("testes ok")


if __name__ == "__main__":
    main()
```

2. O limite de concorrencia e o `max_workers` do pool: sem ele, dez mil arquivos viram dez mil sondagens simultaneas contra o mesmo disco; com ele, a fila espera em memoria, nao em processos. Threads bastam aqui porque a espera e de E/S — CPU-bound pediria processos, como na [aula 15](15-solucoes.md).

4. `stderr` importa para automacao porque o relatorio de sondagens (`stdout`, dado para o proximo programa) nao pode carregar avisos da ferramenta; quem orquestra le codigo de saida e `stderr`, quem consome le `stdout` — a separacao que a [aula 24](24-solucoes.md) provou.

5. Campos de log em producao ao redor da sondagem: arquivo, inicio/fim com duracao, desfecho e codigo, e o estado do checkpoint (quantos pulados). Sem duracao por arquivo, o arquivo lento se esconde na media do lote.

**Autoverificacao:** checkpoint pula na segunda passada, reset ressonda, timestamp registrado e hashes estaveis entre passadas.
