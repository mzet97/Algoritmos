---
layout: default
---

# Pistas da aula 13a

1. O vetor reserva capacidade maior que o tamanho: ler `v[3]` com `size() == 3` mas `capacity() >= 4` cai em memória alocada e válida para o ASan — é UB lógico sem ser memória inválida. Imprima `size()` e `capacity()` lado a lado para ver por que o sanitizer não é oráculo de contrato.
2. O vazamento aparece no fim, como resumo do LeakSanitizer com a pilha da alocação. O retorno antecipado é o caminho que pula o `delete`: RAII corrige porque o destrutor está preso ao escopo, não à ordem dos retornos.
3. A fronteira: `n == SIZE_MAX / tam` ainda cabe; `n == SIZE_MAX / tam + 1` é a primeira recusa. Teste os dois lados da fronteira, não um valor gigante óbvio.
4. Classificar antes de corrigir: literal conhecido que cabe é aceitável com comentário de contrato; entrada externa pede `snprintf` com retorno checado; tamanho desconhecido pede alocação dimensionada pela entrada, com a guarda do exercício 3.
