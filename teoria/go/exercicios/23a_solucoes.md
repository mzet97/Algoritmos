---
layout: default
---

# Soluções comentadas da aula 23a

## 1. Caminho do `put` replicado

Cliente → primário: valida → persiste (fsync) → aplica → responde `ok` → encaminha (melhor esforço, 2 s). Réplica: persiste → aplica → responde (ignorado). Se o encaminhamento falha, nada se perde: o primário já respondeu e o reenvio do cliente carrega o mesmo ID. A invariante é que o log do primário contém tudo que foi respondido como `ok`.

## 2. Morte no meio de 100 escritas

As respondidas antes da morte estão no log e voltam no replay. As em voo (sem resposta) são reenviadas com o mesmo ID: as aplicadas voltam `repetido: true`, as perdidas aplicam de verdade. Final: 100 chaves, valores íntegros, zero duplicatas no estado — verifique com `get` em tudo, nos dois nós.

## 3. Teto de 1 byte

Todo `put` passa do teto na hora de enquadrar: `escreveQuadro` devolve erro antes de qualquer byte viajar. O protocolo responde o tamanho declarado contra o teto — recusa explicada, não conexão muda. Ajuste o teste para esperar a recusa em vez do sucesso.

## 4. Sem `fsync`

Latência cai (sem barreira de durabilidade por escrita), às vezes muito. O preço: queda de energia ou kernel panic perde o que estava só no page cache — e como o cliente já recebeu `ok`, a perda é silenciosa. Benchmark honesto anota `Sync` ligado ou desligado junto do número; sem isso, comparar é trapaça.
