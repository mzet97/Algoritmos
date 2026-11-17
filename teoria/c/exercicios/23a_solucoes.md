# Soluções comentadas da aula 23a

## 1. Sequência com o par caído

`PUT id2 k2 v2` em A: validação passa; sob o mutex, `id2` não foi visto, o registro vai ao log com `fsync` e é aplicado na memória. Fora do mutex, `encaminhar` tenta conectar em B com timeout de 2 s e desiste (conexão recusada: melhor esforço). Só então A responde `OK v2` — um OK **degradado**: durável em A, ausente em B. Quando B renasce, o replay reconstrói apenas o que o log de B tem (`k1`); `k2` continua ausente até um reenvio dirigido (`PUT id2 k2 v2 F` em B, ou novo encaminhamento a partir de A). Quem repete é quem sabe que escreveu: o cliente/roteiro, com o mesmo `id`.

## 2. Cem escritas com B caído

Todas as 100 ficam duráveis em A e ausentes em B. Na volta de B, **as 100 precisam de reenvio dirigido** — nesta arquitetura não há anti-entropia: nenhum nó anuncia "tenho os ids X, Y, Z" nem pede "me faltam estes". O reenvio com o mesmo `id` é seguro porque, se alguma escrita tiver chegado a B antes da queda (encaminhamento em voo), o `id` já visto devolve `REPETIDO` sem reaplicar. Descoberta automática exigiria detector de falhas e protocolo de sincronização de logs — trabalho futuro declarado na aula, não comportamento fingido.

## 3. Responder antes de persistir

A janela da mentira: `responder(OK)` → queda → o registro nunca chegou ao disco. O cliente tem certeza de uma escrita que o replay não conhece. Se o cliente, satisfeito, nunca repetir, a escrita some em silêncio; se repetir com valor diferente (achando que é uma escrita nova), o estado final é o da **segunda** escrita — o oposto do contrato "vale a primeira". Os checks 3 (duplicata devolve a primeira), 11 (replay após reinício) e 13 (reenvio é `REPETIDO` na origem) passam a depender de o processo não cair na janela: sorte, não contrato.

## 4. Partição com escrita bilateral

Com encaminhamento impossibilitado, `PUT idA ka va` em A e `PUT idB kb vb` em B geram logs disjuntos: cada nó está correto segundo o próprio log e divergente segundo o sistema. Na reconciliação, as opções honestas são:

- **Último-escritor-vence (relógio de parede):** simples, converge — e descarta escritas sem avisar ninguém. Relógios de máquinas distintas nem sequer concordam sobre "último".
- **Vetores de versão / ids por nó:** detectam a divergência e a expõem (como fazem os logs aqui, literalmente), mas alguém — aplicação ou humano — precisa escolher o vencedor.
- **Primário eleito por consenso (Raft/Paxos):** só o líder escreve, logo não há dois lados para divergir. É a solução real — e é outro curso: eleição, quorum e ordem total não cabem nesta aula e não foram fingidos nela.

O que esta aula garante continua de pé dentro de cada nó: nada respondido como `OK` se perde no reinício desse nó, e nenhuma duplicata muda o estado duas vezes.
