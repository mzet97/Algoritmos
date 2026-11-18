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

### Por que o oráculo prova (e o que ele não prova)

O teste compara percurso em-ordem contra `sort_unstable` + `dedup` do mesmo multiconjunto em 300 casos — e a força está nos três qualificadores: *mesmo multiconjunto* (a prova é sobre o conteúdo inserido, não sobre sequência imaginada), *300 casos com duplicatas* (duplicata recusada com `false` é ramo próprio, e ramo não testado é ramo presumido) e *oráculo independente* (a std não compartilha código com a BST; bug replicado nos dois lados passaria — por isso o oráculo é a std, revisada por milhares, e não uma segunda implementação própria). O que o oráculo não prova: altura (medida à parte, em 1 000 chaves) e custo (contado à parte, um `Box` por nó). Corretude, forma e custo são três afirmações com três instrumentos — e a aula as mantém separadas porque instrumento único para três perguntas responde nenhuma direito. A indução na invariante (esquerda < nó < direita ⟹ em-ordem crescente) é o argumento; os 300 casos são a execução do argumento — prova sem execução é revisão, execução sem prova é sorte.

### Leitura guiada: três linhas, três instrumentos

A primeira linha (`propriedade ok: BST em-ordem == sort_unstable+dedup (300 casos)`) é a corretude: 300 multiconjuntos com duplicatas, três tamanhos de zero a 59, zero divergência. A segunda (`altura ordenada=1000 embaralhada=20`) é a forma: o mesmo código, duas entradas, dois regimes — e o contraste ordenada/embaralhada prova que a degeneração é propriedade da entrada, não do código. A terceira fecha com o trio que a aula cobra: invariante provada, forma medida, custo declarado. Repare que a altura vem como dois números, não um: `1000` sozinho alarmaria sem referência; `20` ao lado diz o que a mesma árvore faz com entrada honesta — medida sem controle é susto, com controle é diagnóstico.

## Erros comuns

1. **Lutar com o borrow checker na inserção iterativa:** descer com `&mut Link` e reatribuir no `None` é o padrão; tentar guardar ponteiro para o pai reproduz C com atrito. A árvore de ownership quer recursão ou navegação por referência mutável ao elo, não ao nó.
2. **`sort` quando bastava `sort_unstable`:** sem registros com ordem original a preservar, a estabilidade é custo sem comprador; com registros, é o contrário (08a de C++).
3. **Achar que `Drop` recursivo é grátis:** destruir a lista degenerada de 1 000 nós é recursão de 1 000 quadros na destruição também. Altura é limite operacional nas duas pontas.
4. **Medir sem `-O`:** rustc sem otimização mede o modo debug; o nível vai escrito junto do número.
5. **Oráculo escrito pela mesma mão:** segunda implementação própria como "oráculo" replica o bug de interpretação nos dois lados. Oráculo precisa de independência (a std) ou o teste prova consistência, não corretude.
6. **Casos sem duplicata:** se nenhum caso tem chave repetida, o ramo `false` nunca executa — e a invariante "duplicata não entra" fica presumida. Ramo de recusa exige caso que tenta entrar.

## Exercícios, pistas e solução

Exercícios em [12b](exercicios/12b_pistas.md) (pistas) e [soluções](exercicios/12b_solucoes.md): (1) remoção com os três casos e a prova do sucessor; (2) construção balanceada pela mediana a partir do vetor ordenado, com altura medida; (3) reimplementação com arena (`Vec<No>` + índices `Option<usize>`) e contagem de alocações comparada; (4) o contraexemplo mínimo de entrada que degenera a sua BST e a defesa (balanceamento, shuffle de entrada ou outra estrutura) declarada por escrito.

**Checklist:** invariante enunciada e testada contra a std; degeneração medida e explicada; custo `Box` por nó contado contra o vetor; `-D warnings` limpo; limite de recursão declarado.
