# Algoritmos

Soluções de **153 exercícios** de algoritmos em 5 linguagens: **Python**, **Rust**, **Go**, **C** e **C++**.

Organizados em 2 projetos:

| Projeto | Descrição | Exercícios |
|---------|-----------|:----------:|
| [Fundamentos da Computação](Fundamento_da_Computacao/) | Exercícios introdutórios (101–155) | 55 |
| [Algoritmos](Algoritiomos/) | Exercícios por tópico (T1–T8) | 98 |

---

## Fundamentos da Computação

55 exercícios numerados de 101 a 155. Cobrem aritmética, strings, matrizes, frações, PA/PG, recursão básica e mais.

→ [Ver README completo com enunciados e links](Fundamento_da_Computacao/README.md)

## Algoritmos

98 exercícios em 8 tópicos:

| Tópico | Descrição | Exercícios |
|--------|-----------|:----------:|
| 1 | Aquecimento | 13 |
| 2 | Listas Lineares | 28 |
| 3 | Pilhas e Filas | 9 |
| 4 | Recursão | 19 |
| 5 | Árvores | 15 |
| 6 | Ordenação | 7 |
| 7 | Filas de Prioridade / Heaps | 4 |
| 8 | Tabelas Hash | 3 |

→ [Ver README completo com enunciados e links](Algoritiomos/README.md)

---

## Linguagens

Todos os exercícios são resolvidos nas 5 linguagens:

| Linguagem | Diretório (Fundamentos) | Diretório (Algoritmos) |
|-----------|------------------------|----------------------|
| Python | `Fundamento_da_Computacao/python/` | `Algoritiomos/python/` |
| Rust | `Fundamento_da_Computacao/rust/` | `Algoritiomos/rust/` |
| Go | `Fundamento_da_Computacao/go/` | `Algoritiomos/go/` |
| C | `Fundamento_da_Computacao/c/` | `Algoritiomos/c/` |
| C++ | `Fundamento_da_Computacao/cpp/` | `Algoritiomos/cpp/` |

---

## Biblioteca bib1

Cada projeto/linguagem tem uma biblioteca auxiliar com funções de leitura e formatação:

| Linguagem | Funções |
|-----------|---------|
| Python | `entrada()` |
| Rust | `Leitor::entrada()`, `python_float()` |
| Go | `Entrada()`, `PythonFloat()` |
| C | `entrada()`, `print_python_float()` |
| C++ | `entrada()`, `python_float()` |

- **`entrada()`**  lê uma linha de stdin ignorando linhas vazias, retorna EOF
- **`python_float()`**  formata floats no estilo Python (shortest round-trip)

---

## Notas

- 4 exercícios cobrem conceitos similares entre os dois projetos (soma/subtração de dígitos, raiz quadrada), cada um com especificações de I/O diferentes
- Exercícios do Algoritiomos com `@entradagrande` usam inputs especiais para testes de performance
- Listas de dígitos nos Fundamentos usam índice 0 como dígito menos significativo
