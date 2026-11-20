---
layout: default
---

# Aula 23c — Relógios lógicos, causalidade e a partição que não se finge resolver

Você vai ao terminar, você deverá ordenar eventos com relógio de Lamport, detectar concorrência (nem um nem outro aconteceu-antes) com relógios vetoriais, reproduzir uma partição com escrita bilateral divergente, e reconciliá-la por anti-entropia com ordem total declarada — nomeando a escrita descartada, em vez de escondê-la.

a 23a terminou com uma confissão: se os dois nós escreverem durante uma partição, os logs divergem e a arquitetura não sabe quem vence. Esta aula dá nome e ferramenta a cada parte dessa confissão. Relógio de parede não serve: duas máquinas discordam sobre "agora". Relógios lógicos não dizem que horas são — dizem **o que veio antes do quê**, que é a única pergunta que a ordem de eventos precisa responder.

## 1. Contrato antes do código

Simulação determinística em um processo (o transporte é a 23a; aqui o objeto é a ordem):

- **Lamport:** cada evento local incrementa o relógio; ao receber, `relógio = max(meu, recebido) + 1`. Pós-condição: se A aconteceu-antes de B, então `lamport(A) < lamport(B)`. A recíproca é falsa — e a aula testa exatamente onde ela falha.
- **Vetor:** cada nó mantém um componente por nó; escrever incrementa o próprio; receber faz `max` componente a componente. Pós-condição: A aconteceu-antes de B se e somente se o vetor de A é `<=` o de B em tudo e `<` em algum componente. Fora dessa relação, os eventos são **concorrentes**.
- **Reconciliação declarada:** anti-entropia troca os logs inteiros; o estado é reconstruído aplicando os eventos em ordem total `(lamport, autor, seq)` crescente; **o último da ordem vence** por chave. A escrita perdedora é descartada, e descartá-la é a regra publicada, não um acidente.
- **Fora do contrato:** consenso. Nada aqui elege líder, forma quorum ou impede a partição de acontecer. A aula detecta e reconcilia com perda declarada; impedir a perda é Raft/Paxos, outro curso.

## 2. Onde Lamport para e o vetor começa

Lamport dá uma ordem compatível com a causalidade, mas apaga a diferença entre "B viu A" e "B aconteceu depois por acaso": os dois casos recebem números crescentes. Na partição da fase 2, os dois nós escrevem `k` com Lamport 1 e 1 — o empate **denuncia** que algo está errado, mas não diz o quê. O vetor diz: `(1,0)` contra `(0,1)` — nenhum domina o outro, logo são concorrentes, logo houve escrita bilateral sem comunicação. É essa sentença — *concorrentes* — que autoriza a reconciliação a tratar o par como conflito, e não como sequência.

A regra de desempate `(lamport, autor, seq)` é arbitrária e é por isso que ela precisa estar escrita: qualquer ordem total determinística faz os dois nós convergirem, mas cada uma escolhe um perdedor diferente. Convergência é propriedade da regra ser a mesma nos dois lados; a escrita que morre é escolha de quem escreve a regra.

## 3. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/23c_relogios.c) contém o programa completo:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/23c_relogios.c -o /tmp/23c
/tmp/23c
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-sanitize-recover=all teoria/c/exemplos/23c_relogios.c -o /tmp/23c_a
/tmp/23c_a   # identico, sem relatorio de sanitizer
```

Saída esperada (determinística):

```text
lamport: e1=1 e2=3 (causalidade preservada)
particao: A ve k=111, B ve k=222, eventos concorrentes detectados
reconciliacao: ambos convergem para k=222; k=111 descartado pela ordem total (lamport,autor)
pos-sincronizacao: escrita causal k=333 vence nos dois nos
23c ok: Lamport ordena, vetor detecta concorrencia, particao reconciliada com perda declarada
```

Leitura das quatro fases:

1. **Cadeia causal A → B:** B só escreve depois de receber `e1`; o vetor prova (`antes(e1,e2)`), Lamport acompanha (1 < 3). Duplicata de `e1` é reconhecida e ignorada — idempotência da 23a, agora por `(autor, seq)`.
2. **Partição:** `k=111` e `k=222` coexistem, cada nó correto segundo o próprio log. Os vetores declaram os eventos concorrentes: a divergência é detectável mecanicamente, não por inspeção humana dos logs.
3. **Reconciliação:** anti-entropia troca os logs; reaplicar em ordem total faz os dois convergirem a `k=222` — o último da ordem `(1, autor=1)` — e **`k=111` é descartado**. A aula imprime o descarte. Um sistema que reconcilia sem dizer o que perdeu está mentindo com convergência.
4. **Depois da sincronização,** uma escrita causalmente posterior (`k=333`, que viu as duas anteriores) vence nos dois nós sem regra de desempate: causalidade, quando existe, decide sozinha.

### O que a memória faz aqui?

Logs de eventos limitados (64 por nó, com asserção de teto): a simulação recusa crescer além do contratado em vez de estourar em silêncio. Num sistema real o log é o disco da 23a e a anti-entropia troca **resumos** (vetores) antes de trocar eventos que faltam — trocar tudo é a versão didática, declarada como tal.

## 4. Erros comuns e implicações defensivas

1. **Usar relógio de parede para "último vence":** duas máquinas discordam do agora por milissegundos a segundos; o "último" vira sorteio com carimbo. Ordem total precisa de relógio lógico + desempate determinístico.
2. **Achar que Lamport detecta concorrência:** ele ordena empates arbitrariamente e esconde o conflito. Sem vetor (ou mecanismo equivalente), partição bilateral passa despercebida.
3. **Reconciliar sem declarar a perda:** convergir descartando `k=111` sem registrar é corrupção silenciosa com nome bonito. A regra e o descarte são parte do contrato.
4. **Chamar isto de consenso:** não há eleição, quorum nem segurança sob partição contínua. Quem escreve nos dois lados durante a partição perde uma escrita — este é o teorema da aula, não um bug dela.
5. **Anti-entropia sem deduplicação:** trocar logs inteiros sem reconhecer `(autor, seq)` já vistos reaplica eventos e duplica efeitos. Dedup é pré-condição da convergência.
6. **Ordem total diferente em cada nó:** convergência exige a **mesma** regra nos dois lados; "cada um ordena como prefere" diverge para sempre.

## 5. Ponte com Python, JavaScript e as demais trilhas

O mesmo trio aparece nas pontes irmãs: Go declara "distinguir demonstração didática de consenso correto" no G6; Python pede "consistência declarada e partição bilateral" no PY7; C++ para em primário-réplica no CPP-8 e nomeia o que falta. Em JavaScript, relógios vetoriais são o coração de CRDTs e de sincronização offline-first: o cliente que editou sem rede é o nó B desta aula. A pergunta muda de linguagem, não de forma: *quando dois lados escrevem sem se ver, quem decide, e quem é avisado da perda?*

## 6. Exercícios obrigatórios

1. Troque a ordem total para `(lamport, -autor, seq)` (menor autor por último) e preveja o novo vencedor da fase 3 antes de rodar. Confirme e explique por que a convergência não dependeu da sua escolha.
2. Adicione um terceiro nó à simulação e uma partição que isola só ele. Quais vetores crescem, e a anti-entropia par-a-par (A↔B, B↔C, A↔C) converge os três?
3. Faça a fase 2 escrever chaves **diferentes** (`ka` em A, `kb` em B) e mostre que a reconciliação não descarta nada. Conflito é por chave ou por log inteiro? Justifique com a regra de aplicação.
4. Escreva o parágrafo que faltaria para isto virar consenso: eleição, quorum e o que acontece com a escrita do lado minoritário. Sem implementar — especificar já mostra o tamanho do que a aula não fez.

Use [pistas graduais](exercicios/23c_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/23c_solucoes.md) depois de registrar sua própria explicação.

## 7. Ao final, você consegue

Você concluiu a aula se consegue, sem olhar a solução:

- manter Lamport e vetor nas quatro operações (escrever, receber, duplicata, sincronizar);
- decidir, dados dois vetores, se há aconteceu-antes ou concorrência — e o que cada veredito autoriza;
- reproduzir a partição, detectar a concorrência e reconciliar nomeando o descarte;
- explicar por que qualquer ordem total converge e por que a escolha dela é política, não matemática;
- declarar a fronteira com consenso sem implementar nenhum pedaço dele;
- compilar com `-Werror` e passar limpo sob ASan/UBSan.


**Referências primárias:** [Time, Clocks, and the Ordering of Events (Lamport, 1978)](https://lamport.azurewebsites.net/pubs/time-clocks.pdf) para aconteceu-antes e relógios lógicos; [Virtual Time and Global States (Mattern/Fidge)](https://en.wikipedia.org/wiki/Vector_clock) para relógios vetoriais e detecção de concorrência.

**Próxima aula:** 24 — heap real. Ordem de eventos resolvida no papel; a memória onde os logs moram é o próximo problema.

---

### Para se aprofundar

1. Martin Kleppmann, *Designing Data-Intensive Applications* (O'Reilly, ISBN 978-1449373320), caps. 8-9; 2. *Raft paper* (Ongaro e Ousterhout, [raft.github.io](https://raft.github.io/)); 3. W. Richard Stevens et al., *UNIX Network Programming, vol. 1* (Addison-Wesley, 3. ed., ISBN 978-0131411555).
