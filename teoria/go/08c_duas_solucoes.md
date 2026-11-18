---
layout: default
---

# Aula 08c — Duas soluções em Go: BST, ordenação e a análise confrontada

## Objetivos verificáveis

Ao final, você deverá: implementar BST e ordenação confrontando as análises, enunciar invariantes, e distinguir o que cada solução garante.
Mediremos com níveis de proficiência:
- **Nível 1 — Recordar:** `definir invariante de BST e complexidades de ordenação`;
- **Nível 2 — Aplicar:** `diagnosticar quebra de invariante e corrigir`;
- **Nível 3 — Projetar:** `projetar a comparação justificando critérios e vencedor por cenário`.

**Competências SDD:** MAT, ALG. **Pré-requisitos:** [08a](08a_raciocinio_e_algoritmos.md) (busca, invariante, BFS e hash) e 04–05. Esta ponte amplia o G2: duas estratégias para o mesmo problema, correção argumentada e análise confrontada com medição.

**Objetivo observável:** implementar uma BST com inserção recursiva e percurso em ordem, provar o em-ordem contra `sort.Ints` como oráculo em 300 casos, medir a degeneração (altura 1 000 ordenada, ~21 embaralhada) e comparar a ordenação da std com a inserção quadrática em dois tamanhos.

**Contrato:** invariante esquerda < nó < direita; duplicata sem efeito; o em-ordem da BST é igual ao vetor ordenado sem duplicatas (dedup do oráculo). Altura degenerada igual a `n` é resultado esperado e medido, não defeito escondido. O [arquivo-fonte](exemplos/08c_algoritmos.go) é o programa completo:

```bash
go vet teoria/go/exemplos/08c_algoritmos.go
go run -race teoria/go/exemplos/08c_algoritmos.go
# propriedade ok: BST em-ordem == sort.Ints+dedup (300 casos)
# altura ordenada=1000 embaralhada=21
# 08c ok: ...
```

**Leitura:** em Go, o nó é um ponteiro `*no` gerenciado pelo GC: some o `free` manual de C e o `Box` de dono único de Rust, fica o custo que as três compartilham — uma alocação por nó fora do vetor contíguo que a std ordena. A análise assintótica (`O(altura)`, não `O(log n)`) é a mesma frase nas três trilhas porque é a mesma estrutura; o que muda é quem coleta o nó quando ele morre. A medição de ordenação mostra a std vencendo a inserção já em 2 000 elementos por ordens de grandeza: complexidade não é cerimônia, é o número que aparece quando `n` quadruplica.

### Por que `O(altura)` e não `O(log n)` (a frase que precisa de medida)

`O(log n)` vale para BST balanceada; esta BST não promete balanceamento — então a altura é o parâmetro, e `O(altura)` é a análise honesta. A degeneração medida (1000 ordenada, ~21 embaralhada) executa a diferença: mesma estrutura, mesma máquina, fator 50× na altura só pela ordem de entrada. Quem promete `log n` para BST sem balanceamento está citando o caso médio como garantia — e entrada ordenada (logs por timestamp, IDs sequenciais) é exatamente o caso que a produção entrega. O exercício 4 (prever o tamanho da virada antes de medir) treina a direção certa: análise prevê, medição confronta — e quando divergem, a primeira hipótese é "a constante escondeu o termo", não "a teoria errou".

### Leitura guiada: propriedade, forma, confronto

`propriedade ok (300 casos)` é a corretude contra oráculo independente (`sort.Ints` + dedup — a mesma disciplina da 12b de Rust: oráculo da std, não segunda implementação própria). `altura ordenada=1000 embaralhada=21` é a forma com controle: o 1000 sozinho alarmaria, o 21 ao lado diagnostica (propriedade da entrada, não do código). A medição std-vs-inserção é o confronto análise × número: a previsão assintótica (`O(n²)` perde) aparece como ordens de grandeza já em 2 000 elementos — cedo, porque constante da std é pequena e a da inserção carrega alocação por nó.

**Erros comuns:** prometer `log n` sem medir altura; usar recursão de altura sem declarar o limite da degenerada; comparar inserção e std num tamanho só; esquecer que `sort.Ints` não é estável (com chaves iguais indistinguíveis aqui, a estabilidade não entra no teste — com registros, entraria); oráculo escrito pela mesma mão (replica o bug de interpretação); prever depois de medir (profecia retroativa — a previsão vai escrita antes, e o confronto depois).

**Exercícios:** [pistas](exercicios/08c_pistas.md) e [soluções](exercicios/08c_solucoes.md): (1) remoção com três casos; (2) construção balanceada pela mediana e altura provada; (3) busca binária iterativa sobre o vetor ordenado com invariante escrita, confrontada com `sort.Search`; (4) prever, antes de medir, em que tamanho a inserção perde para a std na sua máquina e conferir.

**Checklist:** invariante e terminação escritas; oráculo independente em centenas de casos; degeneração medida; análise prevista antes da medição e confrontada depois; `go vet` limpo e `-race` sem relatório.

---

### Como esta aula será avaliada

| Critério | Peso |
| -------- | ---: |
| Conceitos e vocabulário (nível 1) | 20% |
| Previsão de comportamento e diagnóstico de erros (nível 2) | 40% |
| Projeto e justificativa de decisões (nível 3) | 30% |
| Registro de comandos, saídas e interpretação | 10% |

**Aprovação:** nota mínima 7,0; pelo menos 60% dos pontos devem vir de previsão e diagnóstico (níveis 2 e 3), não apenas de código que executa.

**Para se aprofundar:**

1. Alan Donovan e Brian Kernighan, *The Go Programming Language* (Addison-Wesley, ISBN 978-0134190440); 2. Documentação oficial em [go.dev/doc](https://go.dev/doc/); 3. Referência de pacotes em [pkg.go.dev](https://pkg.go.dev/).
