# Pistas da aula 13a

1. Para a prova, escreva a fatia `v[ini..fim)` antes da primeira iteração. Depois substitua `(ini, fim)` pelos novos limites em cada ramo e use a ordenação para justificar o descarte. No fim, descubra o tamanho da fatia.
2. Marque o slot como apagado sem esvaziar (tombstone) e faça `buscar` pular tombstones mas parar em slot nunca usado. Conte sondagens com a tabela cheia de tombstones e compare com a tabela limpa.
3. Rode com as três sementes e compare. Se alguma falhasse, a semente diria como reproduzir a sequência. Pense no que `srand(time(NULL))` faria com a reprodutibilidade.
4. Quando achar `v[meio] == alvo`, não retorne: grave `resposta = meio` e continue com `fim = meio`. O invariante vira "a resposta está em `v[ini..fim)` ou já registrada à direita". Prove que o laço termina.
