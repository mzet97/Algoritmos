---
layout: default
---

# Aula 19 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../19.md#19-exercicios-obrigatorios). Estrutura criada e executada num diretorio temporario:

```python
import logging
import os
import subprocess
import sys
import tempfile
from pathlib import Path


def main() -> None:
    with tempfile.TemporaryDirectory() as tmp:
        raiz = Path(tmp)
        src = raiz / "src" / "ferramenta"
        testes = raiz / "tests"
        src.mkdir(parents=True)
        testes.mkdir()
        nl = chr(10)
        (src / "__init__.py").write_text("", encoding="utf-8")
        (src / "nucleo.py").write_text(nl.join([
            "import logging, os",
            "log = logging.getLogger('ferramenta')",
            "LIMITE = int(os.environ.get('FERRAMENTA_LIMITE', '10'))",
            "def dobrar(n):",
            "    log.info('dobrando %s', n)",
            "    return n * 2",
            "",
        ]), encoding="utf-8")
        (src / "cli.py").write_text(nl.join([
            "import sys",
            "from ferramenta.nucleo import LIMITE, dobrar",
            "def main():",
            "    valor = int(sys.argv[1]) if len(sys.argv) > 1 else 0",
            "    print(dobrar(valor), 'limite', LIMITE)",
            "    return 0 if valor <= LIMITE else 2",
            "if __name__ == '__main__':",
            "    raise SystemExit(main())",
            "",
        ]), encoding="utf-8")
        (testes / "test_nucleo.py").write_text(nl.join([
            "from ferramenta.nucleo import dobrar",
            "def test_dobrar():",
            "    assert dobrar(21) == 42",
            "",
        ]), encoding="utf-8")

        ambiente = dict(os.environ, PYTHONPATH=str(raiz / "src"), FERRAMENTA_LIMITE="5")
        ok = subprocess.run([sys.executable, "-m", "ferramenta.cli", "3"],
                            capture_output=True, text=True, env=ambiente, timeout=30)
        fora = subprocess.run([sys.executable, "-m", "ferramenta.cli", "9"],
                              capture_output=True, text=True, env=ambiente, timeout=30)
        teste = subprocess.run([sys.executable, "-m", "pytest", "-q", str(testes)],
                               capture_output=True, text=True, env=ambiente, timeout=60, cwd=raiz)
        print("cli dentro do limite:", ok.stdout.strip(), "rc", ok.returncode)
        print("cli fora do limite rc:", fora.returncode, "| pytest rc:", teste.returncode)
        assert ok.returncode == 0 and fora.returncode == 2 and teste.returncode == 0
    print("testes ok")


if __name__ == "__main__":
    main()
```

2 e 4. A config saiu do codigo para `FERRAMENTA_LIMITE` com padrao no modulo que a usa — um lugar so para procurar. O `main()` devolve o codigo e o `SystemExit` mora na borda: testar a CLI vira chamar uma funcao, nao um processo.

5. Para o proximo projeto, a lista minima: testes nas regras que custam dinheiro/dados, logs nas decisoes e nas fronteiras, config externa em tudo que muda entre sua maquina e producao (URLs, limites, segredos — estes, nunca no codigo).

**Autoverificacao:** pacote em `src/` importavel, pytest verde sobre `tests/`, config por ambiente alterando o comportamento e codigos de saida 0/2 observados.
