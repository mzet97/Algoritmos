---
layout: default
---

# Aula 12b — Árvores e ordenação em Rust: `Box`, invariante e o oráculo da std

**Competências SDD:** MAT, ALG, MEM. **Pré-requisitos:** aulas 09–12 e [12a](12a_raciocinio_e_algoritmos.md) (busca e invariante em vetor). Você precisa saber `Option<Box<T>>`, `match` e empréstimos mutáveis.

**Objetivo observável:** implementar uma BST com `Box` por nó, provar que o percurso em ordem devolve as chaves ordenadas contra `sort_unstable` como oráculo em 300 casos, e medir a degeneração (altura 1 000 ordenada contra ~20 embaralhada) declarando o custo de um `Box` por nó.

**Motivação:** em C, a BST da 13b é ponteiro e `malloc` por nó; em Rust, cada nó é um `Box` — dono único, liberação pelo `Drop`, e a recursão de inserção vira navegação por `&mut Link`. A invariante é a mesma; o que muda é quem prova a liberação: o compilador, pela árvore de ownership. O R3 do mapa pedia ordenação, árvores e custo de alocação com afirmações executáveis: é isto.

## Contrato e exemplo

Pré-condição: invariante esquerda < nó < direita. Inserir devolve `false` para duplicata, sem efeito. Pós-condição do percurso em ordem: sequência ordenada crescente — provada por indução na invariante e testada contra a std (`sort_unstable` + `dedup` do mesmo multiconjunto) em 300 casos com tamanhos 0–59 e duplicatas, mais a degeneração medida em 1 000 chaves: entrada ordenada produz altura 1 000 (a BST vira lista e a recursão de `altura`/`Drop` desce 1 000 quadros — limite declarado, como na trilha de C); embaralhada pelo LCG, altura ~20.

O [arquivo-fonte](exemplos/12b_estruturas.rs) é o programa completo, só biblioteca padrão:

```bash
rustc --edition 2024 -O -D warnings teoria/rust/exemplos/12b_estruturas.rs -o /tmp/12b
/tmp/12b
# propriedade ok: BST em-ordem == sort_unstable+dedup (300 casos)
# altura ordenada=1000 embaralhada=20
# 12b ok: ...
```

O custo de memória é a leitura de sistemas: um `Box` por nó são 1 000 alocações para 1 000 chaves, contra um único vetor contíguo que `sort_unstable` ordena em ~0,9 ms (100 mil elementos) sem alocar por elemento. Em Rust, a forma segura de árvore continua sendo a forma cara de memória — `Vec` com índices (arena) é o exercício 3, e é como árvores de produção costumam nascer nesta linguagem.

## Erros comuns

1. **Lutar com o borrow checker na inserção iterativa:** descer com `&mut Link` e reatribuir no `None` é o padrão; tentar guardar ponteiro para o pai reproduz C com atrito. A árvore de ownership quer recursão ou navegação por referência mutável ao elo, não ao nó.
2. **`sort` quando bastava `sort_unstable`:** sem registros com ordem original a preservar, a estabilidade é custo sem comprador; com registros, é o contrário (08a de C++).
3. **Achar que `Drop` recursivo é grátis:** destruir a lista degenerada de 1 000 nós é recursão de 1 000 quadros na destruição também. Altura é limite operacional nas duas pontas.
4. **Medir sem `-O`:** rustc sem otimização mede o modo debug; o nível vai escrito junto do número.

## Exercícios, pistas e solução

Exercícios em [12b](exercicios/12b_pistas.md) (pistas) e [soluções](exercicios/12b_solucoes.md): (1) remoção com os três casos e a prova do sucessor; (2) construção balanceada pela mediana a partir do vetor ordenado, com altura medida; (3) reimplementação com arena (`Vec<No>` + índices `Option<usize>`) e contagem de alocações comparada; (4) o contraexemplo mínimo de entrada que degenera a sua BST e a defesa (balanceamento, shuffle de entrada ou outra estrutura) declarada por escrito.

**Checklist:** invariante enunciada e testada contra a std; degeneração medida e explicada; custo `Box` por nó contado contra o vetor; `-D warnings` limpo; limite de recursão declarado.
