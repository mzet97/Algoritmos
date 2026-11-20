# Auditoria página por página — andaime editorial (interno, não é aula)

Gerado na revisão editorial de 2026-11-20. Mede quantas páginas publicáveis
ainda exibem ao aluno o andaime interno (códigos de competência, objetivo
"observável", níveis de proficiência, pesos/rubrica, "como ler") que deve viver
apenas nos bastidores (`ESPECIFICACAO_SDD.md`, `MAPA_SDD.md`, esta auditoria).

## Método

Busca textual por aula em `teoria/{c,cpp,rust,go,python}/*.md` (excluídos
`prompt.md`, `MAPA_SDD.md`, `indice.md`): `Competências`, `Objetivo observável`,
`Como ler esta aula`, `Nível 1`, `Pesos da avaliação`, `Rubrica/Critério de
aprovação`, `Evidência ao terminar/de aprovação`, mais presença de seção de
exercícios obrigatórios.

## Resultado (2026-11-20, antes do piloto)

| Livro | Aulas | Com andaime | Sem exercícios |
|---|---|---|---|
| C | 55 | 54 | — |
| C++ | 56 | 55 | — |
| Rust | 48 | 47 | — |
| Go | 47 | 46 | — |
| Python | 47 | 46 | — |
| **Total** | **253** | **248** | **27** |

Combinações mais comuns: só níveis de proficiência (94), “como ler” + pesos
(55), pacote completo competências+objetivo+como ler+pesos+rubrica (28).

## Decisão editorial

- Aluno vê: abertura com problema, “o que você precisa já saber” em prosa,
  modelo mental, experimento, onde quebra, prática, “ao final você consegue”,
  referências primárias.
- Bastidor guarda: códigos de competência, matriz aula→exercício→avaliação,
  rubricas/pesos e estados de gate — neste arquivo, em `MAPA_SDD.md` por livro
  e em `AUDITORIA_POR_ARQUIVO.csv`.
- Piloto do formato: C 01, 04, 07, 10a, 14, 23, 31. Depois varredura por livro
  na ordem C, C++, Rust, Go, Python.
