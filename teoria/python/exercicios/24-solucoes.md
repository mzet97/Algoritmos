---
layout: default
---

# Aula 24 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../24.md#12-exercicios-obrigatorios). A CLI e executada como processo de verdade pelo harness:

```python
import subprocess
import sys
import tempfile
from pathlib import Path

CLI = r'''
import argparse, json, sys
from pathlib import Path

VERSAO = "1.2.0"

def main() -> int:
    parser = argparse.ArgumentParser(prog="ferramenta")
    sub = parser.add_subparsers(dest="comando", required=True)
    sub.add_parser("version")
    p_val = sub.add_parser("validate")
    p_val.add_argument("caminhos", nargs="+")
    p_val.add_argument("--config")
    args = parser.parse_args()
    if args.comando == "version":
        print(VERSAO)
        return 0
    limite = 0
    if args.config:
        limite = int(json.loads(Path(args.config).read_text())["limite_bytes"])
    falhas = 0
    for bruto in args.caminhos:
        caminho = Path(bruto)
        if not caminho.exists():
            print(f"ausente: {caminho}", file=sys.stderr)
            falhas += 1
        elif limite and caminho.stat().st_size > limite:
            print(f"grande demais: {caminho}", file=sys.stderr)
            falhas += 1
        else:
            print(f"ok: {caminho}")
    return 1 if falhas else 0

if __name__ == "__main__":
    raise SystemExit(main())
'''


def main() -> None:
    with tempfile.TemporaryDirectory() as tmp:
        raiz = Path(tmp)
        script = raiz / "ferramenta.py"
        script.write_text(CLI, encoding="utf-8")
        (raiz / "pequeno.txt").write_text("abc", encoding="utf-8")
        (raiz / "grande.txt").write_text("x" * 100, encoding="utf-8")
        (raiz / "cfg.json").write_text('{"limite_bytes": 10}', encoding="utf-8")

        def rodar(*args: str) -> subprocess.CompletedProcess:
            return subprocess.run([sys.executable, str(script), *args],
                                  capture_output=True, text=True, timeout=30)

        versao = rodar("version")
        valido = rodar("validate", str(raiz / "pequeno.txt"), "--config", str(raiz / "cfg.json"))
        invalido = rodar("validate", str(raiz / "pequeno.txt"), str(raiz / "grande.txt"),
                         str(raiz / "sumiu.txt"), "--config", str(raiz / "cfg.json"))
        print("version:", versao.stdout.strip(), "rc", versao.returncode)
        print("validate bom rc:", valido.returncode, "| ruim rc:", invalido.returncode)
        print("stdout do ruim so tem o ok:", invalido.stdout.strip().endswith("pequeno.txt"),
              "| stderr carrega os problemas:", "grande demais" in invalido.stderr)
        assert versao.stdout.strip() == "1.2.0" and versao.returncode == 0
        assert valido.returncode == 0 and invalido.returncode == 1
        assert "sumiu" not in invalido.stdout and "ausente" in invalido.stderr
    print("testes ok")


if __name__ == "__main__":
    main()
```

4. `stderr` importa porque em `ferramenta validate ... | processar` o `stdout` vira entrada do proximo programa: misturar diagnostico nele corrompe o dado. Conversa com humano vai para `stderr`; dado para maquina, para `stdout` — e o harness acima prova os dois separados.

5. Codigos minimos de uma CLI interna: 0 deu certo, 1 deu errado no trabalho (validacao falhou), 2 nem rodou (uso/config invalida, a convencao do `argparse`). Tres perguntas, tres codigos — mais que isso vira linguagem que ninguem consulta.

**Autoverificacao:** subcomando `version`, config JSON limitando de verdade, tres caminhos com veredito resumido num codigo e stdout limpo de diagnostico.
