---
layout: default
---

# Aula 06 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../06.md#14-exercicios-obrigatorios). O harness abaixo cria os arquivos de verdade num diretorio temporario e executa/importa cada um:

```python
import subprocess
import sys
import tempfile
from pathlib import Path


def main() -> None:
    with tempfile.TemporaryDirectory() as tmp:
        raiz = Path(tmp)
        nl = chr(10)
        (raiz / "ops.py").write_text(nl.join([
            "def somar(a, b):",
            "    return a + b",
            "",
            'if __name__ == "__main__":',
            '    print("ops executado")',
            "",
        ]), encoding="utf-8")
        (raiz / "msgs.py").write_text(nl.join([
            "def formatar(nome, total):",
            '    return f"{nome}: total={total}"',
            "",
        ]), encoding="utf-8")
        (raiz / "main.py").write_text(nl.join([
            "from ops import somar",
            "from msgs import formatar",
            'print(formatar("Ada", somar(2, 3)))',
            "",
        ]), encoding="utf-8")
        pacote = raiz / "pacote"
        pacote.mkdir()
        (pacote / "__init__.py").write_text("", encoding="utf-8")
        (pacote / "nucleo.py").write_text("VALOR = 42" + nl, encoding="utf-8")

        def rodar(*args: str) -> str:
            r = subprocess.run([sys.executable, *args], cwd=raiz,
                               capture_output=True, text=True, timeout=30)
            assert r.returncode == 0, r.stderr
            return r.stdout.strip()

        print("main usa os dois modulos:", rodar("main.py"))
        print("importar nao executa o bloco __main__:",
              "ops executado" not in rodar("-c", "import ops"))
        print("executar executa:", rodar("ops.py") == "ops executado")
        print("pacote importa com sobrenome:",
              rodar("-c", "from pacote.nucleo import VALOR; print(VALOR)") == "42")

    # 5. Ciclo, em miniatura: a importa b, b importa a -> ImportError.
    # Refatoracao: o que os dois usam (aqui, a constante BASE) desce para um
    # terceiro modulo sem dependencias, e a/b importam dele — nunca um do outro.
    print("testes ok")


if __name__ == "__main__":
    main()
```

5. Dependencia circular nao se resolve com import dentro da funcao (isso esconde o ciclo ate a chamada azarada): resolve-se movendo a parte comum para baixo na hierarquia, como na [aula 08 de Go](../../go/exercicios/08-solucoes.md).

**Autoverificacao:** main orquestrando dois modulos, importar diferente de executar demonstrado e pacote importavel pelo caminho pontilhado.
