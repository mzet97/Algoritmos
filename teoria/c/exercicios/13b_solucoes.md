# Soluções comentadas da aula 13b

## 1. Construção balanceada pela mediana

```c
static No *construir(const int *v, size_t ini, size_t fim) {
    if (ini >= fim) return NULL;
    size_t meio = ini + (fim - ini) / 2;
    No *n = no_novo(v[meio]);
    if (!n) return NULL;
    n->esq = construir(v, ini, meio);
    n->dir = construir(v, meio + 1, fim);
    return n;
}
```

Cada nível consome todas as chaves restantes e divide o trabalho ao meio: a altura é o número de metades até esvaziar, `⌈log2(n+1)⌉` — para 1 000 chaves, 10. A invariante vale porque o vetor está ordenado: tudo antes da mediana é menor, tudo depois é maior.

## 2. Remoção iterativa

Desça com `No **p` começando em `&raiz`: enquanto `*p` existe e a chave difere, avance `p` para `&(*p)->esq` ou `&(*p)->dir`. Achando: folha/um-filho é `*p = filho; free(vitima);`. Dois filhos continuam mais simples copiando o sucessor e removendo o sucessor com a mesma técnica. O teste de propriedade não muda porque o contrato (invariante + conjunto resultante) não mudou — só o mecanismo de descer.

## 3. Estabilidade

Com `<=`, em empate a mescla consome da esquerda, que contém os elementos que vieram antes no vetor original: `seq` permanece crescente dentro de cada chave. Com `<`, o elemento da direita (mais novo) sai primeiro no empate e alguma chave repetida mostra `seq` fora de ordem. O menor contraexemplo: dois registros `(1, seq=0)` e `(1, seq=1)` em metades diferentes na última mescla.

## 4. Custo de alocação

Medido no exemplo: a BST de 1 000 chaves faz 1 000 alocações de nós (~24 bytes úteis cada, mais o overhead do alocador e o arredondamento — tipicamente 32 bytes por bloco) e 1 000 liberações em pós-ordem. O mergesort faz 1 alocação de 4 000 bytes de buffer (e o vetor, outra). Consequência prática: a BST espalha 1 000 blocos pelo heap (fragmentação, aula 24) e cada passo da busca tende a um cache miss; o vetor é contíguo. É por isso que "árvore" em sistemas reais costuma ser larga e compacta (B-tree), e não binária nó-a-nó.
