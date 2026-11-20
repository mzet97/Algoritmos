---
layout: default
---

# Aula 04b — Árvores e ordenação em Python: o preço de um nó-objeto

Uma árvore em Python paga um objeto por nó — com cabeçalho, referências e coletor junto. Medir esse preço contra a alternativa é aprender que estrutura de dados também é decisão de memória.

## Nesta aula

Você vai aprender a implementar árvore explicando o custo do nó-objeto, enunciar invariante de ordenação, e comparar com a `std`.

Você vai implementar uma BST em Python puro, provar o percurso em ordem contra `sorted(set(...))` em 300 casos, medir a degeneração (altura 300 ordenada contra ~20 embaralhada) e explicar por que `sorted` vence qualquer árvore manual nesta linguagem.

**Contrato e exemplo:** invariante esquerda < nó < direita, duplicata sem efeito, em-ordem igual ao oráculo. O [arquivo-fonte](exemplos/04b_arvores.py) é o programa completo:

```bash
python3 teoria/python/exemplos/04b_arvores.py
# propriedade ok: BST em-ordem == sorted(set) em 300 casos
# altura ordenada(300)=300 embaralhada(1000)=20
# 04b ok: ...
```

**Leitura:** cada nó Python é um objeto com `__slots__` (sem ele, um dicionário por nó), três referências e overhead de objeto — dezenas de bytes para guardar um inteiro que, numa lista, custa um ponteiro para um `int` compartilhado. `sorted` (Timsort, em C) ordena 200 mil elementos em ~0,024 s sem criar nó nenhum. A conclusão madura do PY2: BST manual em Python é instrumento para aprender a invariante e a degeneração; em produção, `sorted`, `bisect` e `dict` são as respostas, e a aula que não diz isso forma quem reimplementa pior o que a std já fez. A degeneração, essa sim, é universal: entrada ordenada vira lista em qualquer linguagem (e aqui ainda cobra `setrecursionlimit` para a altura ser sequer mensurável).

### O preço do nó-objeto, em bytes e em quadros

Conte o custo de um nó: objeto (`__slots__` com três campos) + o `int` da chave (objeto próprio, ~28 bytes) + três referências — contra 8 bytes por elemento num vetor contíguo. O fator ~10× por elemento não é pessimismo, é o exercício 3 (`sys.getsizeof` com e sem `__slots__`): sem `__slots__`, cada nó carrega um `__dict__` inteiro para guardar três campos que nunca mudam de forma — dicionário por nó para esquema fixo é a forma mais cara de guardar o mais simples. E a degeneração cobra em quadros: altura 300 com limite de recursão padrão (1000) passa, mas por margem que some se o teste crescer — o `setrecursionlimit` da aula não é gambiarra, é declaração de que a profundidade da recursão é função da entrada, e função da entrada se declara. Em C a mesma degeneração estoura a pilha nativa; em Python ela levanta `RecursionError` legível — mesma física, diagnóstico melhor, e a aula conta os dois.

### Leitura guiada: propriedade, forma, veredito de produção

`propriedade ok (300 casos)` é a corretude contra oráculo da std (`sorted(set(...))` — independência, como nas trilhas irmãs). `altura ordenada(300)=300 embaralhada(1000)=20` é a forma com controle: degeneração provada na entrada adversária, sanidade na honesta. O veredito (`04b ok`) fecha com a conclusão de produção escrita na Leitura: a BST manual perde para `sorted` por ordens de grandeza, e perder aqui é o resultado esperado — a árvore era o instrumento, nunca o produto.

**Erros comuns:** prometer `O(log n)` sem medir altura; esquecer o limite de recursão ao demonstrar a degenerada (o `RecursionError` é parte da demonstração, não um acidente); comparar BST manual com `sorted` como se fosse disputa justa; usar nó sem `__slots__` e não contar o custo; medir altura só na entrada honesta (sem a ordenada, a degeneração fica presumida — e presumida é o estado em que ela morde); concluir "Python é lento" em vez de "nó-objeto é caro" (a lentidão mora na representação escolhida, e `sorted` na mesma linguagem prova isso a cada run).

**Exercícios:** [pistas](exercicios/04b_pistas.md) e [soluções](exercicios/04b_solucoes.md): (1) remoção com três casos; (2) mediana sobre vetor ordenado com altura medida; (3) medir memória por nó (`sys.getsizeof` com e sem `__slots__`) para 10 mil nós; (4) reescrever a busca com `bisect` sobre lista ordenada e declarar quando a lista vence a árvore.

**Checklist:** invariante testada contra oráculo; degeneração medida nas duas ordens; custo por nó contado; conclusão de produção escrita (std primeiro); `-m py_compile` limpo via regress.

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

1. Luciano Ramalho, *Python Fluente* (Novatec/O'Reilly, ISBN 978-1492056355); 2. Documentação oficial em [docs.python.org/3](https://docs.python.org/3/); 3. *Tutorial oficial* em [docs.python.org/3/tutorial](https://docs.python.org/3/tutorial/).
