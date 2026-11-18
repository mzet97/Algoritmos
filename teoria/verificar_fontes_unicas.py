#!/usr/bin/env python3
"""Fontes unicas (gate SDD): extrai os exemplos completos das aulas numeradas,
monta cada um como fonte unica (projetos multiarquivo reunem header+impl+main
da propria aula), compila com -Werror e executa com stdin fechado e timeout.

Classificacoes honestas, por bloco:
- executado: compilou e saiu 0;
- falha_intencional: exemplo rotulado de falha na aula (ex.: divisao por zero);
- cliente_eco: cliente de rede executado contra um servidor eco descartavel
  em namespace de rede proprio;
- servidor: sobe e fica ouvindo (timeout com o processo vivo, em namespace de
  rede quando a aula faz bind, para nao disputar portas da maquina);
- sem_main: aula sem programa completo em bloco (ponte/projeto cobre o tema).

Uso: python3 teoria/verificar_fontes_unicas.py c|cpp
Sai 0 somente se nenhum bloco divergir da classificacao observada.
"""
import re
import subprocess
import sys
import tempfile
from pathlib import Path

RAIZ = Path(__file__).resolve().parent

VOLUMES = {
    "c": {
        "pasta": "c",
        "cercas": ["c"],
        "extensao": ".c",
        "compilador": ["gcc", "-std=c11"],
        "extras": ["-lm"],
        "falha_intencional": {("11", 0): "divisao por zero (SIGFPE) da aula"},
        "servidores": {"15", "20"},
        "clientes_eco": set(),
        "fixtures": {"08": "entrada.txt", "16": "entrada.txt"},
        "argumentos": {("32", 0): ["nome=ada", "--get", "nome"]},
        "padrao_por_aula": {},
    },
    "cpp": {
        "pasta": "cpp",
        "cercas": ["cpp", "c++"],
        "extensao": ".cpp",
        "compilador": ["g++", "-std=c++20"],
        "extras": [],
        "falha_intencional": {},
        "servidores": {"31"},
        "clientes_eco": {"18"},
        "fixtures": {"16": "dados.txt"},
        "argumentos": {("16", 0): ["dados.txt"], ("32", 0): ["set", "nome", "ada"]},
        "padrao_por_aula": {"11": ["g++", "-std=c++23"]},
    },
}

ECO = (
    "import socket\n"
    "s = socket.socket()\n"
    "s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)\n"
    "s.bind(('127.0.0.1', 8080))\n"
    "s.listen(1)\n"
    "c, _ = s.accept()\n"
    "dados = c.recv(1024)\n"
    "c.sendall(dados)\n"
    "c.close()\n"
)


def blocos(texto: str, cercas: list[str]) -> list[str]:
    encontrados: list[str] = []
    for cerca in cercas:
        encontrados.extend(re.findall(rf"```{re.escape(cerca)}\n(.*?)```", texto, re.S))
    return encontrados


def tem_main(codigo: str) -> bool:
    return re.search(r"\b(?:int|void)\s+main\s*\(", codigo) is not None


def corpo_main(codigo: str) -> bool:
    return re.search(r"\b(?:int|void)\s+main\s*\([^)]*\)\s*\{", codigo) is not None


def montar_fontes(indice_main: int, todos: list[str], destino: Path,
                  extensao: str) -> list[Path] | None:
    """Devolve as fontes a compilar juntas, ou None se o bloco e trecho.

    Projeto multiarquivo: o header e o bloco com o guard derivado do nome
    incluido (stack.h -> STACK_H); header e implementacao sao os vizinhos
    mais recentes anteriores ao main na aula (a aula pode trazer versoes
    evolutivas do mesmo projeto, e misturar versoes nao compila).
    """
    principais = [b for b in todos if tem_main(b) and corpo_main(b)]
    if indice_main >= len(principais):
        return None
    main = principais[indice_main]
    includes_locais = re.findall(r'#include\s+"([^"]+)"', main)
    if not includes_locais:
        fonte = destino / f"main{extensao}"
        fonte.write_text(main, encoding="utf-8")
        return [fonte]
    pos_main = todos.index(main)
    fontes: list[Path] = []
    for nome_header in includes_locais:
        guarda = re.sub(r"[^A-Z0-9]", "_", nome_header.upper())
        candidatos = [b for b in todos[:pos_main]
                      if f"#ifndef {guarda}" in b and not tem_main(b)]
        if not candidatos:  # header com #pragma once em vez de guard
            candidatos = [b for b in todos[:pos_main]
                          if "#pragma once" in b and not tem_main(b)]
        if not candidatos:
            return None
        (destino / nome_header).write_text(candidatos[-1], encoding="utf-8")
        impls = [b for b in todos[:pos_main]
                 if f'#include "{nome_header}"' in b and not tem_main(b)
                 and "#ifndef" not in b]
        if not impls:
            return None
        fonte = destino / f"impl_{Path(nome_header).stem}{extensao}"
        fonte.write_text(impls[-1], encoding="utf-8")
        fontes.append(fonte)
    fonte_main = destino / f"main{extensao}"
    fonte_main.write_text(main, encoding="utf-8")
    fontes.append(fonte_main)
    return fontes


def verificar(volume: str) -> list[str]:
    cfg = VOLUMES[volume]
    falhas: list[str] = []
    resumo: dict[str, list[str]] = {}
    with tempfile.TemporaryDirectory() as tmp:
        base = Path(tmp)
        eco = base / "eco.py"
        eco.write_text(ECO, encoding="utf-8")
        for numero in range(1, 33):
            aula = f"{numero:02d}"
            caminho = RAIZ / cfg["pasta"] / f"{aula}.md"
            if not caminho.exists():
                continue
            todos = blocos(caminho.read_text(encoding="utf-8", errors="ignore"),
                           cfg["cercas"])
            principais = [b for b in todos if tem_main(b) and corpo_main(b)]
            if not principais:
                resumo[aula] = ["sem_main"]
                continue
            classes: list[str] = []
            for indice in range(len(principais)):
                trabalho = base / f"{aula}_{indice}"
                trabalho.mkdir()
                fontes = montar_fontes(indice, todos, trabalho, cfg["extensao"])
                if fontes is None:
                    classes.append("trecho")
                    continue
                executavel = trabalho / "prog"
                compilador = cfg["padrao_por_aula"].get(aula, cfg["compilador"])
                compilacao = subprocess.run(
                    [*compilador, "-Wall", "-Wextra", "-Wpedantic", "-Werror",
                     "-o", str(executavel), *[str(f) for f in fontes],
                     *cfg["extras"], "-pthread"],
                    capture_output=True, text=True, timeout=180)
                if compilacao.returncode != 0:
                    classes.append("NAO_COMPILA")
                    falhas.append(
                        f"{volume}/{aula}[{indice}] nao compila: {compilacao.stderr[:200]}")
                    continue
                if aula in cfg["fixtures"]:
                    (trabalho / cfg["fixtures"][aula]).write_text(
                        "linha um\nlinha dois\n", encoding="utf-8")
                args = cfg["argumentos"].get((aula, indice), [])
                if aula in cfg["servidores"]:
                    comando = ["unshare", "-rn", str(executavel), *args]
                elif aula in cfg["clientes_eco"]:
                    comando = ["unshare", "-rn", "bash", "-c",
                               f"ip link set lo up; python3 {eco} & sleep 1; exec {executavel}"]
                else:
                    comando = [str(executavel), *args]
                try:
                    corrida = subprocess.run(
                        comando, capture_output=True, text=True, timeout=6,
                        stdin=subprocess.DEVNULL, cwd=trabalho)
                    if (aula, indice) in cfg["falha_intencional"]:
                        classes.append(
                            "falha_intencional" if corrida.returncode != 0 else "DIVERGE")
                        if corrida.returncode == 0:
                            falhas.append(
                                f"{volume}/{aula}[{indice}] deveria falhar e saiu 0")
                    elif corrida.returncode == 0:
                        classes.append(
                            "cliente_eco" if aula in cfg["clientes_eco"] else "executado")
                    else:
                        classes.append(f"exit{corrida.returncode}")
                        falhas.append(
                            f"{volume}/{aula}[{indice}] exit {corrida.returncode}: "
                            f"{corrida.stdout[:120]} {corrida.stderr[:120]}")
                except subprocess.TimeoutExpired:
                    if aula in cfg["servidores"]:
                        classes.append("servidor")
                    else:
                        classes.append("TIMEOUT")
                        falhas.append(
                            f"{volume}/{aula}[{indice}] timeout sem ser servidor")
            resumo[aula] = classes
    for aula, classes in resumo.items():
        print(f"{volume}/{aula}: {', '.join(classes)}")
    return falhas


def main() -> int:
    if len(sys.argv) != 2 or sys.argv[1] not in VOLUMES:
        print("uso: verificar_fontes_unicas.py c|cpp", file=sys.stderr)
        return 2
    falhas = verificar(sys.argv[1])
    if falhas:
        print("FALHAS:")
        for falha in falhas:
            print(" -", falha)
        return 1
    print("fontes unicas: tudo conforme o classificado")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
