---
layout: default
---

# Pistas da aula 12b

1. Remocao: desca com &mut Link como na insercao; dois filhos copiam o sucessor (minimo da direita) e removem o sucessor. A prova de que o em-ordem nao muda e a mesma da 13b de C.
2. Mediana como raiz a partir do vetor ordenado: construa recursivamente das metades; a altura medida deve ser o teto de log2(n+1).
3. Arena: Vec<No> onde cada No guarda indices Option<usize> em vez de Box. Uma alocacao do Vec (com realocacoes amortizadas) contra n Boxes: conte com um contador global de alocacoes ou pelo tempo de construir/destruir.
4. O contraexemplo e a entrada ordenada (altura n). Defesas: balancear (AVL/rubro-negra), embaralhar antes de inserir (treap com prioridade aleatoria) ou trocar a estrutura (BTreeMap da std, que ja e balanceada).
