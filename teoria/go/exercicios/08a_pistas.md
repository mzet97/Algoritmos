# Pistas da aula 08a

1. Para a prova, escreva a fatia `valores[ini:fim]` antes da primeira iteração. Depois substitua `(ini, fim)` pelos novos limites em cada ramo do `switch` e use a ordenação para justificar o descarte. No fim, descubra o tamanho da fatia.
2. Marque o vértice como visitado **antes** do `append` na fila. Conte quantas vezes cada vértice sai da fila. Se o destino não existe, o que esvazia a fila?
3. Rode com as três sementes e compare. Se alguma falhasse, a semente diria como reproduzir a sequência. Pense no que `rand.NewSource(time.Now().UnixNano())` faria com a reprodutibilidade.
4. O invariante da soma é: antes da iteração com contador `i`, o acumulado vale `1+...+(i-1)`. Para abortar a recursão, passe um `n` com mais quadros que a pilha da goroutine comporta e observe: não há `recover` que salve estouro de pilha.
