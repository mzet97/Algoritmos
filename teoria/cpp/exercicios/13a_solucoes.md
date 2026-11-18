---
layout: default
---

# Soluções comentadas da aula 13a

## 1. Leitura fora dentro da capacidade

`std::vector` separa `size()` (elementos válidos) de `capacity()` (memória alocada). Ler além do `size()` mas dentro da `capacity()` toca memória que pertence ao processo: o ASan não reporta porque, para o alocador, nada de inválido aconteceu. É UB pelo contrato da biblioteca, não pela página de memória. Moral: sanitizer detecta a classe "memória inválida"; a classe "índice inválido dentro da alocação" é contrato — pega-se com `at()`, com revisão ou com testes de fronteira, não com ASan sozinho.

## 2. Vazamento por retorno antecipado

Com ponteiro cru, qualquer `return` entre o `new` e o `delete` vaza; o LeakSanitizer lista a alocação e a pilha onde nasceu. A correção por `unique_ptr` não é "lembrar de deletar": é mover a liberação para o destrutor, que roda em **todas** as saídas do escopo, incluindo exceção. É o mesmo argumento da regra de zero da aula de ownership: quem aloca num recurso com dono não escreve liberação nenhuma.

## 3. Guarda de alocação

```cpp
static std::optional<std::size_t> bytes_para(std::size_t n, std::size_t tam) {
    if (tam != 0 && n > SIZE_MAX / tam) return std::nullopt;
    return n * tam;
}
```

A fronteira testável: `bytes_para(SIZE_MAX / tam, tam)` devolve valor; `+ 1` devolve `nullopt`. Sem a guarda, `n * tam` transborda para um número pequeno e a alocação "bem-sucedida" mente sobre o tamanho disponível — o overflow acontece depois, longe da causa.

## 4. Classificação de `strcpy`

Três classes, três destinos: (a) literal conhecido que cabe no destino fixo — aceitável, com o contrato comentado (é o caso da aula 25 de C, anotado na revisão); (b) entrada externa com destino fixo — `snprintf` e retorno comparado com o tamanho, truncamento vira erro; (c) tamanho descoberto só em execução — dimensionar o destino pela entrada (`std::string`, vetor) em vez de copiar para um palpite. Corrigir (a) como se fosse (b) é ruído; tratar (b) como (a) é a vulnerabilidade.
