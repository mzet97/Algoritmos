---
layout: default
---

# Pistas da aula 23a

1. Siga um `PUT` pelo código: `tratar` → checagem de `id` sob o mutex → `persistir` → aplicar → `encaminhar` → responder. Com o par caído, marque em qual passo o tempo é gasto (timeout de conexão) e o que já está durável quando o cliente recebe `OK`.
2. Reenvio dirigido é o cliente (ou o roteiro) repetir, no nó que faltou, o `PUT ... F` com o `id` original. Conte no log de cada nó: linhas presentes nos dois, só em A, só em B. Sem gossip nem detector de falhas, nenhum nó sabe o que o outro perdeu — por isso a convergência é dirigida, não automática.
3. Com responder→persistir, a janela é: cliente recebe `OK`, processo cai antes do `fsync`. No replay, o `id` não existe; o reenvio reaplica como novo — e se o valor mudou, o estado final depende de qual escrita a sorte preservou. O check que fica à mercê da sorte é o da duplicata/replay (3, 11 e 13).
4. Para simular a partição, suba os nós com a porta do par apontando para uma porta morta (ou `0` com encaminhamento desligado por nó) e escreva chaves diferentes nos dois. Compare os logs linha a linha: a divergência é literal. Reconciliações possíveis: último-escritor-vence por relógio (perde escrita sem avisar), vetores de versão (detecta, não resolve), ou eleger um primário (exige consenso). Cada uma declara o que perde.
