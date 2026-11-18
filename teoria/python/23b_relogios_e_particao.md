---
layout: default
---

# Aula 23b — Relógios e partição bilateral em Python

**Competências SDD:** DST, NET, CON. **Pré-requisitos:** [23a](23a_sistemas_distribuidos.md) (IDs, log com fsync, dedup, kill -9, replay) e [19a](19a_protocolos_tcp.md). Esta ponte completa o PY7: relógios/ordem e consistência declarada sobre a base da 23a.

**Objetivo observável:** ordenar eventos por Lamport, detectar concorrência por vetores, reproduzir a partição bilateral (`k=111` × `k=222`) e reconciliar por anti-entropia com ordem total publicada, nomeando o descarte — na quarta linguagem em que este teorema aparece (C 23c, Go 23b, Rust 23b análogo), porque reconhecer o padrão é a competência.

**Contrato e exemplo:** simulação determinística (o transporte é a 23a). O [arquivo-fonte](exemplos/23b_relogios.py):

```bash
python3 teoria/python/exemplos/23b_relogios.py
# particao: A ve k=111, B ve k=222, vetores detectam concorrencia
# reconciliacao: ambos k=222; k=111 descartado pela ordem (lamport,autor)
# 23b ok: ...
```

**Leitura:** em Python, a estrutura inteira cabe numa página de `dataclass` — e é essa a armadilha pedagógica: a facilidade de escrever esconde que nada aqui é consenso. Lamport ordena sem detectar; o vetor detecta sem resolver; a ordem total resolve descartando. A política de consistência deste sistema é, por escrito: *eventual, com último-pela-ordem-total vencendo e perda declarada*. Quem não consegue escrever essa frase sobre o próprio sistema não sabe qual sistema construiu — em nenhuma linguagem, e Python apenas torna mais fácil construir sem perceber.

### A divisão de trabalho dos três mecanismos (quarta passagem, primeira em Python)

Lamport dá a ordem total `(lamport, autor)` que permite reaplicar os logs dos dois lados na mesma sequência e convergir — sem ele, cada nó reaplicaria na sua chegada e os estados divergiriam para sempre. O vetor decide *quando* a ordem total precisa descartar: comparável significa causa-efeito (aplica-se em sequência, nada se perde); incomparável significa concorrência (aplica-se a regra, um deles cai). A anti-entropia é o transporte da decisão: trocar logs, deduplicar por `(autor, seq)`, reaplicar em ordem crescente. Três mecanismos, três perguntas respondidas — "em que ordem?", "há conflito?", "como os dois ficam iguais?" — e a partição bilateral (`k=111` × `k=222`) é o caso em que as três respostas aparecem juntas: ordem total diz 222 depois de 111, vetor diz concorrência, anti-entropia aplica e imprime o descarte. Quem decora só a ordem total constrói reconciliação que converge para o errado em causa-efeito (o aviso da trilha Go, aqui repetido porque a facilidade do `dataclass` faz parecer que está tudo bem).

### Leitura guiada: partição, reconciliação, veredito

A primeira linha documenta o conflito detectado (dois valores, vetores incomparáveis — concorrência, não atraso). A segunda documenta a resolução com a regra citada (`pela ordem (lamport,autor)`) e o perdedor nomeado (`k=111 descartado`) — reconciliação sem descarte nomeado é merge silencioso, e merge silencioso é perda escondida. O veredito fecha com o rótulo honesto: simulação determinística, transporte da 23a, consenso fora. Repare que a saída não mostra valores convergindo "por mágica": cada passo (detectar, ordenar, descartar) tem linha própria, porque cada passo é mecanismo distinto que pode quebrar distinto.

**Erros comuns:** `datetime.now()` como carimbo de ordem entre máquinas; chamar a reconciliação de "sincronização" sem publicar a regra; deduplicar só por conteúdo (dois eventos iguais de autores diferentes são dois eventos); achar que a simulação prova o comportamento do serviço da 23a sob rede real; aplicar desempate sem consultar o vetor (ordem total em causa-efeito descarta efeito como se fosse concorrência — convergência para o estado errado com regra bonita); escrever a política de consistência depois de reconciliar (a frase de uma linha vem antes, no contrato — reconciliação sem política declarada é comportamento à procura de significado).

**Exercícios:** [pistas](exercicios/23b_pistas.md) e [soluções](exercicios/23b_solucoes.md): (1) transportar os relógios para dentro do serviço da 23a (carimbar cada PUT) e mostrar um log real ordenado; (2) terceiro nó isolado; (3) chaves distintas sem descarte; (4) o parágrafo de consenso que falta (eleição, quorum, minoritário).

**Checklist:** os três mecanismos separados e nomeados; descarte impresso; simulação rotulada como simulação; política de consistência escrita em uma frase.
