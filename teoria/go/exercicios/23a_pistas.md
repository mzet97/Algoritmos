---
layout: default
---

# Pistas da aula 23a

1. Desenhe duas colunas (primário, réplica) e uma linha do tempo: valide → persiste → aplica → responde → encaminha. Marque onde cada seta pode falhar e quem repete o quê em cada caso.
2. Mate o processo no meio com `kill -9 <pid>` em vez de esperar o roteiro: conte respostas `ok`, reenvie tudo com os mesmos IDs e compare o estado final com o log.
3. Baixe `maxQuadro` para 1 e rode o roteiro: o primeiro `put` já estoura o teto. Leia o erro: ele diz o tamanho declarado e o teto — protocolo que explica a recusa.
4. Troque `file.Sync()` por nada e rode 1000 escritas cronometradas antes/depois. A diferença é o preço da durabilidade: quem paga, paga em latência; quem não paga, paga na queda.
