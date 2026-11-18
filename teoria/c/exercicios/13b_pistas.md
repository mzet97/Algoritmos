---
layout: default
---

# Pistas da aula 13b

1. Mediana como raiz: com vetor ordenado `v[ini..fim)`, escolha `meio = ini + (fim - ini) / 2` como nó e recursa nas duas metades. A altura de cada lado difere de no máximo 1 — some os dois lados para provar o teto logarítmico.
2. Iterativo: desça guardando o endereço do ponteiro (`No **`) que aponta para o nó atual, como na inserção do exemplo. Com `No **`, o splice dos casos folha/um-filho é uma atribuição só, sem caso especial para a raiz.
3. Estabilidade mora num único operador: na mescla, empate deve consumir da metade esquerda primeiro (`<=`). Com `<`, o da direita passa na frente e a `seq` dentro da chave sai decrescente em algum empate — monte o menor vetor com duplicatas que mostra isso.
4. Conte: BST = 1 000 `malloc`/`free` de ~24 bytes (dois ponteiros + `int` + padding); vetor + mergesort = 2 alocações grandes. Fragmentação e localidade de cache são a aula 24 — chegue lá com os dois números medidos, não estimados.
