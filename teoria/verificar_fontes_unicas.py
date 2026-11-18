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


def secao_completa(texto: str) -> str | None:
    casamento = re.search(
        r"^## .*(?:[Ee]xemplo [Cc]ompleto|[Pp]rojeto [Cc]ompleto).*$", texto, re.M)
    if not casamento:
        return None
    resto = texto[casamento.start():]
    fim = re.search(r"\n## ", resto[len(casamento.group(0)):])
    if not fim:
        return resto
    return resto[:len(casamento.group(0)) + fim.start()]


ENTRADAS = {"02": "42\n"}


def verificar_rust() -> list[str]:
    """Rust: a fonte unica e a secao de exemplo/projeto completo da aula.

    Aulas 18-32 sao projetos Cargo (servidores, SQLx, gRPC, wgpu, eBPF e os
    projetos 30-32): a execucao delas vive nos projetos e nas solucoes do
    gate, nao num rustc de arquivo unico — aqui sao classificadas, nao
    compiladas. Os trechos fora da secao completa incluem erros intencionais
    de borrow/ownership, que sao o conteudo das aulas iniciais.
    """
    falhas: list[str] = []
    with tempfile.TemporaryDirectory() as tmp:
        base = Path(tmp)
        for numero in range(1, 33):
            aula = f"{numero:02d}"
            caminho = RAIZ / "rust" / f"{aula}.md"
            if not caminho.exists():
                print(f"rust/{aula}: sem aula numerada")
                continue
            if numero >= 18:
                print(f"rust/{aula}: projeto_cargo (execucao nos projetos/solucoes)")
                continue
            secao = secao_completa(caminho.read_text(encoding="utf-8", errors="ignore"))
            if secao is None:
                falhas.append(f"rust/{aula}: sem secao de exemplo completo")
                continue
            trabalho = base / aula
            trabalho.mkdir()
            if aula == "06":
                marcadores = re.findall(r"`(src/[^`]+)`:?\s*\n+```rust\n(.*?)```",
                                        secao, re.S)
                (trabalho / "Cargo.toml").write_text(
                    '[package]\nname = "calculadora"\nversion = "0.1.0"\nedition = "2024"\n',
                    encoding="utf-8")
                for caminho_fonte, codigo in marcadores:
                    destino = trabalho / caminho_fonte
                    destino.parent.mkdir(parents=True, exist_ok=True)
                    destino.write_text(codigo, encoding="utf-8")
                corrida = subprocess.run(
                    ["cargo", "run", "--quiet"], cwd=trabalho,
                    capture_output=True, text=True, timeout=300,
                    stdin=subprocess.DEVNULL)
                classe = "executado" if corrida.returncode == 0 else "NAO_COMPILA"
                if corrida.returncode != 0:
                    falhas.append(f"rust/06 cargo: {corrida.stderr[:200]}")
                print(f"rust/{aula}: {classe} (projeto multiarquivo via cargo)")
                continue
            blocos = re.findall(r"```rust\n(.*?)```", secao, re.S)
            com_main = [b for b in blocos if "fn main" in b]
            if not com_main:
                falhas.append(f"rust/{aula}: secao completa sem main")
                continue
            codigo = com_main[-1]
            if re.search(r"^use (tokio|axum|serde|sqlx|tonic|wgpu)\b", codigo, re.M):
                print(f"rust/{aula}: projeto_cargo (crate externa na secao completa)")
                continue
            fonte = trabalho / "main.rs"
            fonte.write_text(codigo, encoding="utf-8")
            executavel = trabalho / "prog"
            compilacao = subprocess.run(
                ["rustc", "--edition", "2024", "-O", "-D", "warnings",
                 "-o", str(executavel), str(fonte)],
                capture_output=True, text=True, timeout=300)
            if compilacao.returncode != 0:
                falhas.append(
                    f"rust/{aula} nao compila: {compilacao.stderr[:300]}")
                print(f"rust/{aula}: NAO_COMPILA")
                continue
            try:
                corrida = subprocess.run(
                    [str(executavel)], input=ENTRADAS.get(aula, ""),
                    capture_output=True, text=True, timeout=30, cwd=trabalho)
                if corrida.returncode == 0:
                    print(f"rust/{aula}: executado")
                else:
                    falhas.append(
                        f"rust/{aula} exit {corrida.returncode}: {corrida.stderr[:200]}")
                    print(f"rust/{aula}: exit{corrida.returncode}")
            except subprocess.TimeoutExpired:
                falhas.append(f"rust/{aula} timeout")
                print(f"rust/{aula}: TIMEOUT")
    return falhas


PROJETOS_EXTERNOS_GO = {
    "21": "gorilla/websocket (executado na solucao com modulo)",
    "22": "lib/pq + Postgres (solucao executou com SQLite)",
    "23": "gRPC com codigo gerado (executado na solucao com protoc)",
}
PROCESSOS_CONTINUOS_GO = {"26", "27"}
ARGUMENTOS_GO = {("28", 0): ["grande.log"], ("31", 0): ["-files", "alvo.txt"]}


def verificar_go() -> list[str]:
    """Go: cada bloco com package+main compila como modulo descartavel e roda.

    Trechos sem `package` sao fragmentos; 08 monta o projeto multi-package da
    aula; servidores sobem em namespace de rede; 26/27 sao processos continuos
    (timeout com o processo vivo e a classificacao); 21-23 dependem de
    infraestrutura externa e foram executados nas solucoes do gate.
    """
    falhas: list[str] = []
    with tempfile.TemporaryDirectory() as tmp:
        base = Path(tmp)
        for numero in range(1, 32):
            aula = f"{numero:02d}"
            caminho = RAIZ / "go" / f"{aula}.md"
            if not caminho.exists():
                continue
            todos = re.findall(r"```go\n(.*?)```",
                               caminho.read_text(encoding="utf-8", errors="ignore"), re.S)
            principais = [b for b in todos if "func main()" in b]
            if aula in PROJETOS_EXTERNOS_GO:
                print(f"go/{aula}: projeto_externo ({PROJETOS_EXTERNOS_GO[aula]})")
                continue
            if not principais:
                print(f"go/{aula}: sem_main")
                continue
            classes: list[str] = []
            for indice, codigo in enumerate(principais):
                if not codigo.lstrip().startswith("package "):
                    classes.append("trecho")
                    continue
                trabalho = base / f"{aula}_{indice}"
                trabalho.mkdir()
                (trabalho / "grande.log").write_text("linha de log\n" * 100, encoding="utf-8")
                (trabalho / "dados.bin").write_bytes(b"abcdefgh" * 128)
                (trabalho / "alvo.txt").write_text("conteudo\n", encoding="utf-8")
                if aula == "08":
                    (trabalho / "go.mod").write_text(
                        "module exemplo.com/curso-go\n\ngo 1.23\n", encoding="utf-8")
                    (trabalho / "main.go").write_text(codigo, encoding="utf-8")
                    for bloco in todos:
                        pacote = re.match(r"package (\w+)", bloco.lstrip())
                        if pacote and pacote.group(1) != "main":
                            pasta = trabalho / pacote.group(1)
                            pasta.mkdir(exist_ok=True)
                            (pasta / f"{pacote.group(1)}.go").write_text(
                                bloco, encoding="utf-8")
                else:
                    (trabalho / "main.go").write_text(codigo, encoding="utf-8")
                    subprocess.run(["go", "mod", "init", "x"], cwd=trabalho,
                                   capture_output=True, timeout=60)
                    if "golang.org/x/sys" in codigo:
                        subprocess.run(["go", "get", "golang.org/x/sys@latest"],
                                       cwd=trabalho, capture_output=True, timeout=300)
                compilacao = subprocess.run(
                    ["go", "build", "-race", "-o", "prog", "."],
                    cwd=trabalho, capture_output=True, text=True, timeout=300)
                if compilacao.returncode != 0:
                    classes.append("NAO_COMPILA")
                    falhas.append(f"go/{aula}[{indice}] nao compila: {compilacao.stderr[:250]}")
                    continue
                servidor = "ListenAndServe" in codigo and "httptest" not in codigo
                args = ARGUMENTOS_GO.get((aula, indice), [])
                if servidor:
                    comando = ["unshare", "-rn", str(trabalho / "prog"), *args]
                else:
                    comando = [str(trabalho / "prog"), *args]
                try:
                    corrida = subprocess.run(
                        comando, capture_output=True, text=True, timeout=6,
                        stdin=subprocess.DEVNULL, cwd=trabalho)
                    if corrida.returncode == 0:
                        classes.append("executado")
                    else:
                        classes.append(f"exit{corrida.returncode}")
                        falhas.append(
                            f"go/{aula}[{indice}] exit {corrida.returncode}: "
                            f"{corrida.stdout[:120]} {corrida.stderr[:120]}")
                except subprocess.TimeoutExpired:
                    if servidor:
                        classes.append("servidor")
                    elif aula in PROCESSOS_CONTINUOS_GO:
                        classes.append("processo_continuo")
                    else:
                        classes.append("TIMEOUT")
                        falhas.append(f"go/{aula}[{indice}] timeout inesperado")
            print(f"go/{aula}: {', '.join(classes)}")
    return falhas


def main() -> int:
    if len(sys.argv) != 2 or sys.argv[1] not in ("c", "cpp", "rust", "go"):
        print("uso: verificar_fontes_unicas.py c|cpp|rust|go", file=sys.stderr)
        return 2
    if sys.argv[1] == "rust":
        falhas = verificar_rust()
    elif sys.argv[1] == "go":
        falhas = verificar_go()
    else:
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
