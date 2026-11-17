---
layout: default
---

# Aula 08c — Duas soluções em Go: BST, ordenação e a análise confrontada

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

**Erros comuns:** prometer `log n` sem medir altura; usar recursão de altura sem declarar o limite da degenerada; comparar inserção e std num tamanho só; esquecer que `sort.Ints` não é estável (com chaves iguais indistinguíveis aqui, a estabilidade não entra no teste — com registros, entraria).

**Exercícios:** [pistas](exercicios/08c_pistas.md) e [soluções](exercicios/08c_solucoes.md): (1) remoção com três casos; (2) construção balanceada pela mediana e altura provada; (3) busca binária iterativa sobre o vetor ordenado com invariante escrita, confrontada com `sort.Search`; (4) prever, antes de medir, em que tamanho a inserção perde para a std na sua máquina e conferir.

**Checklist:** invariante e terminação escritas; oráculo independente em centenas de casos; degeneração medida; análise prevista antes da medição e confrontada depois; `go vet` limpo e `-race` sem relatório.
