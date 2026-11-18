---
layout: default
---

# Soluções comentadas da aula 23a

## 1. Linha do tempo da escrita com o par caído

Cliente → A: valida → persiste (fsync) → aplica → encaminha (falha: conexão recusada, descarta) → responde `ok`. O log de A tem a operação; o de B, não. O cliente repete nada — recebeu `ok`. Na volta de B, o reenvio com o mesmo ID preenche a lacuna; B responde `ok` sem duplicar nada, porque o ID já... não, espera: B nunca viu esse ID, então B *aplica de verdade* desta vez. Idempotência não significa "nunca aplica", significa "aplicar duas vezes tem o mesmo efeito que uma".

## 2. 100 escritas com morte no meio

Tipicamente: N1 respostas `ok` imediatas (antes da morte), o resto falha no `connect`. Cada falha é reenviada após o reinício com o mesmo ID: as já aplicadas voltam `repetido: true` (prova de que o log sobreviveu), as perdidas aplicam de verdade. Contagem final: 100 chaves, 100 valores, zero duplicatas no estado — verifique com `get` em todas.

## 3. Atraso maior que o timeout

O cliente recebe resposta do primário, que aplicou e persistiu *antes* de encaminhar. O par lento recebe a operação 2 s depois e a aplica então — convergência atrasada, não perdida. A lição: timeout no encaminhamento protege a latência do cliente; a convergência fica eventualmente consistente porque o ID permite reenvio e o log permite replay.

## 4. Escrita bilateral e reconciliação honesta

Logs: A tem `k=X`, B tem `k=Y`, nenhum viu o outro. Regra simples: último por relógio físico vence — mas relógios divergem, então a regra honesta é declarar o conflito em vez de escondê-lo: marque `k` como em conflito, exponha ambas as versões com seus IDs e deixe o operador (ou a regra de negócio documentada) decidir. Sem quorum, reconciliação automática silenciosa é adivinhação com boa caligrafia.
