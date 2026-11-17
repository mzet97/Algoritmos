#!/usr/bin/env python3
"""Consistencia transversal da masterclass (Fase 6).

Verifica, sem tocar em nada:
1. Todo .md de aula nos cinco livros tem linha no CSV (mapas excluidos:
   sao documentos de navegacao, nao aulas auditadas).
2. Toda linha do CSV aponta para um arquivo existente.
3. Todo link Markdown relativo fora de blocos de codigo resolve.
4. Nenhum estado editorial alem de `rascunho` sem gate (hoje: todos).

Sai 0 se tudo confere; 1 com a lista de divergencias.
"""
import csv
import pathlib
import re
import sys

RAIZ = pathlib.Path(__file__).parent
LIVROS = ("c", "cpp", "rust", "go", "python")
EXCLUIDOS = {"MAPA_SDD.md"}


def main() -> int:
    problemas: list[str] = []
    with open(RAIZ / "AUDITORIA_POR_ARQUIVO.csv", newline="", encoding="utf-8") as f:
        linhas = [r for r in csv.reader(f) if len(r) == 6]
    no_csv = {r[0]: r for r in linhas}

    arquivos = set()
    for lang in LIVROS:
        for md in (RAIZ / lang).glob("*.md"):
            if md.name in EXCLUIDOS:
                continue
            rel = str(md.relative_to(RAIZ.parent))
            arquivos.add(rel)
            if rel not in no_csv:
                problemas.append(f"sem linha no CSV: {rel}")
    for rel in no_csv:
        if rel.startswith("teoria/") and rel not in arquivos and \
                pathlib.Path(rel).name not in EXCLUIDOS and not (RAIZ.parent / rel).exists():
            problemas.append(f"CSV sem arquivo: {rel}")
    estados = {r[5] for r in linhas}
    if estados - {"rascunho", "estado_editorial"}:
        problemas.append(f"estados inesperados: {estados}")

    for md in RAIZ.rglob("*.md"):
        texto = md.read_text(encoding="utf-8", errors="ignore")
        texto = re.sub(r"```.*?```", "", texto, flags=re.S)
        texto = re.sub(r"`[^`]*`", "", texto)
        for m in re.finditer(r"\[[^\]]*\]\(([^)]+)\)", texto):
            dest = m.group(1).split("#")[0].strip()
            if dest and "://" not in dest and not (md.parent / dest).exists():
                problemas.append(f"link quebrado: {md.relative_to(RAIZ)} -> {dest}")

    if problemas:
        print(f"CONSISTENCIA FALHOU ({len(problemas)}):")
        for p in problemas[:30]:
            print(" -", p)
        return 1
    print(f"CONSISTENCIA_OK aulas={len(arquivos)} linhas_csv={len(linhas)} "
          f"estados={sorted(estados)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
