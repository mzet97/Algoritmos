#!/usr/bin/env python3
"""Fontes unicas (gate SDD): extrai os exemplos completos das aulas numeradas,
monta cada um como fonte unica (projetos multiarquivo reunem header+impl+main
da propria aula), compila com -Werror e executa com stdin fechado e timeout.

Classificacoes honestas, por bloco:
- executado: compilou e saiu 0;
- falha_intencional: exemplo rotulado de falha na aula (ex.: divisao por zero);
- trecho: fragmento sem corpo de main ou bloco de projeto ja montado por outra via;
- servidor: sobe e fica ouvindo (timeout com o processo vivo, em namespace de
  rede quando a aula faz bind, para nao disputar portas da maquina);
- sem_main: aula sem programa completo em bloco (ponte/projeto cobre o tema).

Uso: python3 teoria/verificar_fontes_unicas.py c
Sai 0 somente se nenhum bloco divergir da classificacao observada.
"""
import re
import subprocess
import sys
import tempfile
from pathlib import Path

RAIZ = Path(__file__).resolve().parent

# Exemplos cuja falha E o conteudo da aula (mecanismo explicado nela).
FALHA_INTENCIONAL = {
    ("11", 0): "divisao por zero (SIGFPE) demonstrada na aula",
}
# Aulas cujos programas completos fazem bind e ficam ouvindo.
SERVIDORES = {"15", "20"}
# Aulas que leem arquivo de entrada nomeado: a fixture e criada no cwd.
FIXTURES = {"08": "entrada.txt", "16": "entrada.txt"}
# Programas completos que exigem argumentos de linha de comando.
ARGUMENTOS = {("32", 0): ["nome=ada", "--get", "nome"]}


def blocos(texto: str, linguagem: str) -> list[str]:
    return re.findall(rf"```{linguagem}\n(.*?)```", texto, re.S)


def tem_main(codigo: str) -> bool:
    return re.search(r"\b(?:int|void)\s+main\s*\(", codigo) is not None


def corpo_main(codigo: str) -> bool:
    casamento = re.search(r"\b(?:int|void)\s+main\s*\([^)]*\)\s*\{", codigo)
    return casamento is not None


def montar_fontes(aula: str, indice_main: int, todos: list[str], destino: Path) -> list[Path] | None:
    """Devolve os .c a compilar juntos, ou None se o bloco e trecho/projeto.

    Projeto multiarquivo: o header e o bloco com o guard derivado do nome
    incluido (stack.h -> STACK_H); header e implementacao sao os vizinhos
    imediatamente anteriores ao main na aula (a aula pode trazer versoes
    evolutivas do mesmo projeto, e misturar versoes nao compila).
    """
    principais = [b for b in todos if tem_main(b) and corpo_main(b)]
    if indice_main >= len(principais):
        return None
    main = principais[indice_main]
    includes_locais = re.findall(r'#include\s+"([^"]+)"', main)
    if not includes_locais:
        fonte = destino / "main.c"
        fonte.write_text(main, encoding="utf-8")
        return [fonte]
    pos_main = todos.index(main)
    fontes: list[Path] = []
    for nome_header in includes_locais:
        guarda = re.sub(r"[^A-Z0-9]", "_", nome_header.upper())
        candidatos = [b for b in todos[:pos_main]
                      if f"#ifndef {guarda}" in b and not tem_main(b)]
        if not candidatos:
            return None
        (destino / nome_header).write_text(candidatos[-1], encoding="utf-8")
        impls = [b for b in todos[:pos_main]
                 if f'#include "{nome_header}"' in b and not tem_main(b)
                 and "#ifndef" not in b]
        if not impls:
            return None
        fonte = destino / f"impl_{nome_header}.c"
        fonte.write_text(impls[-1], encoding="utf-8")
        fontes.append(fonte)
    fonte_main = destino / "main.c"
    fonte_main.write_text(main, encoding="utf-8")
    fontes.append(fonte_main)
    return fontes


def verificar_c() -> list[str]:
    falhas: list[str] = []
    resumo: dict[str, list[str]] = {}
    with tempfile.TemporaryDirectory() as tmp:
        base = Path(tmp)
        for numero in range(1, 33):
            aula = f"{numero:02d}"
            caminho = RAIZ / "c" / f"{aula}.md"
            if not caminho.exists():
                continue
            todos = blocos(caminho.read_text(encoding="utf-8", errors="ignore"), "c")
            principais = [b for b in todos if tem_main(b) and corpo_main(b)]
            if not principais:
                resumo[aula] = ["sem_main"]
                continue
            classes: list[str] = []
            for indice in range(len(principais)):
                trabalho = base / f"{aula}_{indice}"
                trabalho.mkdir()
                fontes = montar_fontes(aula, indice, todos, trabalho)
                if fontes is None:
                    classes.append("trecho")
                    continue
                executavel = trabalho / "prog"
                compilacao = subprocess.run(
                    ["gcc", "-std=c11", "-Wall", "-Wextra", "-Wpedantic", "-Werror",
                     "-o", str(executavel), *[str(f) for f in fontes], "-lm", "-pthread"],
                    capture_output=True, text=True, timeout=120)
                if compilacao.returncode != 0:
                    classes.append("NAO_COMPILA")
                    falhas.append(f"c/{aula}[{indice}] nao compila: {compilacao.stderr[:200]}")
                    continue
                if aula in FIXTURES:
                    (trabalho / FIXTURES[aula]).write_text(
                        "linha um\nlinha dois\n", encoding="utf-8")
                comando = [str(executavel), *ARGUMENTOS.get((aula, indice), [])]
                if aula in SERVIDORES:
                    comando = ["unshare", "-rn", str(executavel)]
                try:
                    corrida = subprocess.run(
                        comando, capture_output=True, text=True, timeout=4,
                        stdin=subprocess.DEVNULL, cwd=trabalho)
                    if (aula, indice) in FALHA_INTENCIONAL:
                        classes.append("falha_intencional" if corrida.returncode != 0 else "DIVERGE")
                        if corrida.returncode == 0:
                            falhas.append(f"c/{aula}[{indice}] deveria falhar e saiu 0")
                    elif corrida.returncode == 0:
                        classes.append("executado")
                    else:
                        classes.append(f"exit{corrida.returncode}")
                        falhas.append(
                            f"c/{aula}[{indice}] exit {corrida.returncode}: "
                            f"{corrida.stdout[:120]} {corrida.stderr[:120]}")
                except subprocess.TimeoutExpired:
                    if aula in SERVIDORES:
                        classes.append("servidor")
                    else:
                        classes.append("TIMEOUT")
                        falhas.append(f"c/{aula}[{indice}] timeout sem ser servidor")
            resumo[aula] = classes
    for aula, classes in resumo.items():
        print(f"c/{aula}: {', '.join(classes)}")
    return falhas


def main() -> int:
    if len(sys.argv) != 2 or sys.argv[1] != "c":
        print("uso: verificar_fontes_unicas.py c", file=sys.stderr)
        return 2
    falhas = verificar_c()
    if falhas:
        print("FALHAS:")
        for falha in falhas:
            print(" -", falha)
        return 1
    print("fontes unicas: tudo conforme o classificado")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
