---
layout: default
---

# Aula 13b — Árvores binárias de busca e ordenação estável

Você vai ao terminar, você deverá implementar uma árvore binária de busca (BST) com inserção, busca e os três casos de remoção; provar que o percurso em ordem entrega as chaves ordenadas; medir a degeneração da árvore quando a entrada já vem ordenada; e implementar um mergesort estável confrontado com `qsort` como oráculo em 500 casos determinísticos.

a 13a fechou com busca binária sobre vetor ordenado: rápido para buscar, caro para inserir no meio. A BST promete busca e inserção no mesmo caminho — *se* a forma da árvore colaborar. Esta aula é sobre a distância entre a promessa (`O(log n)`) e o contrato real (`O(altura)`, e a altura depende da ordem de chegada). É a ponte que faltava no módulo C3 entre "estrutura que funciona" e "estrutura cujo custo você consegue defender".

## 1. Contrato antes do código

BST desta aula (chaves `int`, sem duplicatas):

- **Pré-condição:** toda chave na subárvore esquerda é menor que a do nó; toda chave na direita é maior. É esta invariante — não a forma — que as operações preservam.
- **Pós-condição de `inserir`:** devolve `1` se inseriu, `0` se a chave já existia (sem efeito), `-1` em falha de alocação (árvore intacta).
- **Pós-condição de `remover`:** devolve `1` se removeu, `0` se ausente. Folha e um filho são *splices* (o filho sobe); dois filhos copiam o sucessor — o mínimo da subárvore direita — e removem o sucessor, que tem no máximo um filho.
- **Custo declarado:** busca/inserção/remoção custam `O(altura)`. Altura de árvore degenerada é `n`; de árvore "embaralhada típica", perto de `log n` — medido abaixo, não presumido.

## 2. Em ordem é ordenado: a prova em três linhas

O percurso em ordem visita esquerda → nó → direita. Pela invariante, tudo na esquerda é menor que o nó e tudo na direita é maior. Indução na altura: se os percursos das subárvores saem ordenados, a concatenação esquerda + nó + direita também sai. É por isso que o teste de propriedade desta aula pode usar `qsort` como oráculo: **em-ordem da BST deve ser igual ao vetor ordenado das chaves distintas**. Se divergir, ou a inserção quebrou a invariante, ou a remoção perdeu uma chave.

A remoção com dois filhos merece o mesmo cuidado: copiar o sucessor preserva a invariante porque o sucessor é maior que tudo na esquerda e menor que tudo que restou na direita. Remover o sucessor depois é seguro porque ele não tem filho esquerdo (é o mínimo), logo cai num dos casos simples.

## 3. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/13b_arvores.c) contém exatamente o programa testado (BST + mergesort no mesmo arquivo):

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/13b_arvores.c -o /tmp/13b
/tmp/13b
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-sanitize-recover=all teoria/c/exemplos/13b_arvores.c -o /tmp/13b_asan
/tmp/13b_asan   # mesmo resultado, sem relatório de sanitizer
```

Saída esperada (a altura embaralhada é determinística: LCG de semente fixa sobre 1 000 chaves):

```text
altura ordenada=1000 embaralhada=24 (esperado ~log2(1000)=10)
13b ok: BST (3 casos de remocao), degeneracao medida, mergesort==qsort
```

Três fatos para confrontar com a teoria:

1. **Inserir 0..999 em ordem produz altura 1000.** A BST virou uma lista ligada cara: cada busca é `O(n)`. Nenhuma linha de código está "errada" — a entrada explorou o pior caso que o contrato declarou.
2. **As mesmas chaves embaralhadas pelo LCG dão altura 24.** Não é `log2(1000) ≈ 10`: altura esperada de BST aleatória é cerca de `2·ln(n) ≈ 14` em média, com cauda para cima. Medir 24 com semente fixa é honesto; prometer 10 seria propaganda.
3. **Mergesort com `<=` na mescla é estável** (iguais mantêm a ordem original) e custa `O(n)` de buffer auxiliar, alocado uma vez com a conta verificada. `qsort` não promete estabilidade — quando a estabilidade importa, o contrato escolhe o algoritmo, não o hábito.

### O que a memória faz aqui?

Cada nó é um `malloc` individual: 1 000 chaves, 1 000 alocações, e `bst_destruir` libera em pós-ordem (filhos antes do pai — inverter vaza ou usa memória liberada). A recursão de `altura`/`destruir` sobre a árvore degenerada desce 1 000 quadros: funciona aqui, mas é o limite declarado — entrada adversária de 1 000 000 ordenados estouraria a pilha antes de qualquer busca. Árvores de produção se balanceiam (AVL, rubro-negra) ou se constroem a partir de vetor ordenado; esta aula mede o problema antes de apresentar a cura como exercício.

## 4. Erros comuns e implicações defensivas

1. **Prometer `O(log n)` sem falar de altura:** o custo é `O(altura)`; quem controla a entrada controla a altura. Entrada ordenada é o ataque mais barato contra BST ingênua.
2. **Remover dois filhos trocando ponteiros sem cuidado:** copiar o sucessor e remover o sucessor é simples e provável; religar subárvores na mão é onde nascem ciclos e vazamentos.
3. **Esquecer o caso duplicata:** inserir chave existente sem contrato ou duplica silenciosamente ou corrompe a contagem. Aqui: `0`, sem efeito, testado.
4. **Destruir em pré-ordem:** liberar o pai antes dos filhos perde os ponteiros. Pós-ordem, sempre.
5. **Comparador de `qsort` com subtração:** `x - y` transborda nos extremos. `(x > y) - (x < y)`, como na 13a.
6. **Recursão sem limite sobre entrada adversária:** profundidade é altura; altura degenerada é `n`. Declare o limite ou itere.

## 5. Ponte com Python e JavaScript

Python não tem BST na biblioteca padrão: usa `dict` (hash) e `bisect` sobre lista; JavaScript idem com `Map` e arrays. A invariante "esquerda < nó < direita" é a mesma ideia que sustenta índices de banco de dados (B-trees são BSTs largas): você não implementa uma todo dia, mas decide custos com ela toda semana. Em C, a forma, a altura e cada `free` são seus — é por isso que a degeneração aparece aqui como número medido, e não como surpresa em produção.

## 6. Exercícios obrigatórios

1. Rebalanceie: construa uma BST balanceada a partir do vetor ordenado 0..999 (raiz = mediana, recursivo) e meça a altura. Prove que ela é `⌈log2(n+1)⌉`.
2. Implemente remoção iterativa (sem recursão) e mostre, com o mesmo teste de propriedade, que o em-ordem não muda.
3. Faça o mergesort ordenar registros `(chave, seq)` só pela chave e demonstre a estabilidade: `seq` crescente dentro de cada chave. Troque `<=` por `<` e mostre o teste que falha.
4. Calcule quantas alocações a BST faz para 1 000 chaves contra 1 alocação do vetor + mergesort. Relacione com a aula 24 (fragmentação).

Use [pistas graduais](exercicios/13b_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/13b_solucoes.md) depois de registrar sua própria explicação.

## 7. Ao final, você consegue

Você concluiu a aula se consegue, sem olhar a solução:

- enunciar a invariante da BST e provar que o em-ordem sai ordenado;
- executar os três casos de remoção e justificar o sucessor;
- prever e medir a altura para entrada ordenada e embaralhada, explicando a diferença;
- declarar o custo como `O(altura)` e dizer quem controla a altura;
- compilar com `-Werror`, passar no oráculo `qsort` e nos sanitizers;
- explicar o preço de memória da BST (um `malloc` por nó) contra o do mergesort (um buffer).


**Referências primárias:** *Introduction to Algorithms* (Cormen et al.), capítulos de BST e mergesort, para invariante e estabilidade; `man qsort` para o contrato do oráculo (sem promessa de estabilidade).

**Próxima aula:** 14 — concorrência. A invariante desta aula é sequencial; a próxima pergunta é o que resta dela quando duas threads a tocam ao mesmo tempo.

---

### Para se aprofundar

1. Brian Kernighan e Dennis Ritchie, *C Programming Language* (Prentice Hall, 2. ed., ISBN 978-0131103627); 2. *C11 standard draft N1570* ([open-std.org](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf)); 3. Randall Bryant e David O'Hallaron, *Computer Systems: A Programmer's Perspective* (Pearson, 3. ed., ISBN 978-0134092669).
