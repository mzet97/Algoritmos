---
layout: default
---

# Aula 23c: pistas graduais

Volte aos [exercícios](../23c.md#7-exercícios-obrigatórios) antes de abrir as soluções.

1. O `size !=` retorna antes de tocar no conteúdo: o tempo varia com o tamanho, não com o acerto. Tamanho de token não é segredo — viaja em todo pacote. A propriedade que importa (não vazar *qual* byte difere) vale porque o laço sempre percorre tudo quando os tamanhos igualam.
2. Com quorum 3, 2 respostas não bastam: `respostas < QUORUM` devolve `Leitura{}` = ausente, e o `assert` da fase 1 cai. "Todos" pune o sistema pela queda de um — o mesmo teorema da 23d, agora na leitura.
3. A cura copia a vencedora para todas: troque a escolha da vencedora para `replicas[2]` e o convergente muda de 222 para 333, sem conflito. Nenhum vetor "prefere" 333 — a regra prefere. Quorum + vetor detectam; a regra de cura resolve; as três são peças separadas.
4. Três escritas isoladas dão relógios incomparáveis dois a dois: `(1,0,0)` vs `(0,1,0)` — nem um lado domina. O quorum conta 3 respostas (quorum ok) mas declara `conflito=1` em vez de servir o primeiro. Primário sozinho serviria 111 calado; quorum com vetor expõe.
