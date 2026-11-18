---
layout: default
---

# Aula 13 (Go): pistas graduais

Volte aos [exercicios](../13.md#12-exercicios-obrigatorios) antes de abrir as solucoes.

1. `os.ReadFile` devolve o conteudo inteiro; erro de abertura nao e arquivo vazio.
2. `bufio.Scanner` le linha a linha — confira `scanner.Err()` no fim.
3. JSON sai com `json.Marshal` e as tags `json:"..."` mandam nos nomes.
4. A volta e `json.Unmarshal` para uma struct do mesmo desenho.
5. `filepath.Join` usa o separador da plataforma e limpa o caminho; concatenar nao faz nenhum dos dois.
