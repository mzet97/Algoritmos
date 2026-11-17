# Pistas da aula 23c

1. Inverter o desempate muda só quem é o **último** da ordem total. Reaplique mentalmente a fase 3 com `(1, autor=1)` vindo antes de `(1, autor=0)`: quem é aplicado por último agora? A convergência sobrevive porque a regra continua idêntica nos dois nós.
2. Com três nós, vetores têm três componentes. Na partição do nó C, os vetores de A e B crescem juntos e o de C cresce sozinho. Anti-entropia par-a-par converge se o grafo de encontros for conexo ao longo do tempo — desenhe os encontros antes de codar.
3. Olhe `aplicar`: ela procura **a chave** do evento. Eventos de chaves diferentes não se sobrescrevem. Se a sua reconciliação descartou `ka` ou `kb`, o defeito está na reconstrução do estado, não na ordem.
4. Para o parágrafo de consenso, responda: quem pode escrever quando a rede parte no meio? Quantos precisam concordar para uma escrita valer (quorum)? O que acontece com quem escreveu no lado que perdeu a eleição? Três respostas honestas já especificam o abismo entre esta aula e Raft.
