---
layout: default
---

# Pistas da aula 23a

1. Desenhe a linha do tempo de uma escrita: cliente → A (valida, persiste, aplica, responde) → A encaminha → B (valida, persiste, aplica). Marque onde cada falha possível deixa o sistema e quem repete o quê.
2. Mate o primário no meio de 100 escritas sequenciais e conte quantas voltam `ok` na primeira tentativa, quantas precisam de reenvio e quantas o replay recupera. Explique cada grupo.
3. Com atraso de 2 s e timeout de 0,5 s, o cliente recebe resposta — de quem? (Dica: quem aplicou antes de encaminhar?) Explique por que o par lento não aparece na resposta.
4. Escreva k=X no nó A isolado e k=Y no nó B isolado, depois religue. Compare os logs linha a linha: qual regra decide o valor final, e o que ela perde?
