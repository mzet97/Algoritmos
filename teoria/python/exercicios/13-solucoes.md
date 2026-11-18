---
layout: default
---

# Aula 13 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../13.md#19-exercicios-obrigatorios).

```python
import json
import tempfile
from pathlib import Path


def main() -> None:
    with tempfile.TemporaryDirectory() as tmp:
        raiz = Path(tmp)
        texto = raiz / "notas.txt"
        linhas = ["linha um", "linha dois", "linha tres"]
        texto.write_text(chr(10).join(linhas) + chr(10), encoding="utf-8")
        lidas = texto.read_text(encoding="utf-8").splitlines()
        print("pathlib leu:", lidas)
        assert lidas == linhas

        dados = {"nome": "Ada", "nota": 9.5}
        arq_json = raiz / "dados.json"
        arq_json.write_text(json.dumps(dados), encoding="utf-8")
        carregado = json.loads(arq_json.read_text(encoding="utf-8"))
        if "nome" not in carregado:
            raise ValueError("chave obrigatoria ausente: nome")
        assert carregado == dados

        binario = raiz / "bruto.bin"
        binario.write_bytes(bytes([0, 1, 2, 255]))
        assert binario.read_bytes() == bytes([0, 1, 2, 255])
        print("binario ida e volta:", list(binario.read_bytes()))
    print("testes ok")


if __name__ == "__main__":
    main()
```

4. Validar na leitura e o "parse, nao valide depois" da [aula 11](11-solucoes.md): o JSON que entra e conferido na fronteira, e o resto do programa confia no dicionario.

**Autoverificacao:** tres linhas de volta, JSON com chave conferida e bytes identicos sem passar por texto.
