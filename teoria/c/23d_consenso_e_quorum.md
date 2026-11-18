---
layout: default
---

# Aula 23d — Eleição, quorum e o commit que a minoria não faz

**Competências SDD:** DST, SEC e MAT. **Pré-requisitos:** aulas [23a](23a_sistemas_distribuidos.md) e [23c](23c_relogios_e_particao.md). Você precisa saber o que aquelas aulas garantiram (log com `fsync`, idempotência, relógios lógicos, reconciliação com perda declarada) e o que elas declararam não garantir: impedir a perda. Esta aula impede uma classe de perda — a confirmação falsa — sem prometer o consenso completo.

**Objetivo observável:** ao terminar, você deverá eleger um líder com maioria de votos, confirmar um comando só quando ele estiver na maioria, demonstrar que um nó isolado não elege nem confirma nada, e curar a partição com um termo novo que descarta a escrita não confirmada — nomeando o descarte.

**Motivação:** a 23a terminou com dois nós que convergem; a 23c terminou com dois nós que divergem e reconciliam com perda declarada. Falta a pergunta que um sistema real precisa responder antes de confirmar qualquer coisa ao cliente: *quantos nós precisam concordar para que uma escrita conte?* A resposta desta aula é uma só palavra — **maioria** — e todo o resto é consequência dela.

## 1. Contrato antes do código

Simulação determinística em um processo, três nós, sem rede e sem relógio (o transporte é chamada de função; aqui o objeto é a regra):

- **Termo:** número de mandato que só cresce. Quem vê termo maior adota o termo e volta a seguidor. Termo é a época da eleição, não o tempo do mundo.
- **Eleição:** um candidato vota em si, pede voto aos demais; cada eleitor vota uma vez por termo e só em candidato pelo menos tão atualizado quanto ele (termo e tamanho do log). Com 3 nós, o quorum é 2. Um voto só não elege ninguém.
- **Replicação e commit:** o líder anexa o comando no próprio log, copia para os alcançáveis e só então marca como confirmado — se e somente se a entrada estiver na **maioria**. Confirmar é dizer ao cliente "pode contar com isso"; sem maioria, não se diz.
- **Partição:** o lado de 1 nó não atinge quorum para nada: não elege, não confirma. A escrita solitária dele existe no disco dele, mas não é confirmação — e a cura a descarta em favor do log confirmado do líder.
- **Fora do contrato:** eleição com tempo (timeouts, heartbeat), log compaction, membership change, leitura linearizável e prova de segurança do Raft/Paxos. O que esta aula prova é menor e mesmo assim útil: sem maioria não há confirmação, e confirmação de maioria sobrevive a uma cura.

## 2. Por que maioria, e não "todos" nem "qualquer um"?

"Todos" trava a primeira queda: um nó lento veta o sistema inteiro. "Qualquer um" confirma a primeira escrita solitária — e duas partições confirmam duas verdades opostas. A maioria é o único subconjunto que se intercepta consigo mesmo: quaisquer duas maiorias de 3 nós compartilham pelo menos um nó, e esse nó carrega a escrita confirmada de uma eleição para a outra. É essa interseção que impede duas confirmações conflitantes no mesmo termo e que obriga a cura a preservar o confirmado. O preço aparece na fase 2 do exemplo: o lado minoritário fica indisponível para escrita confirmada — disponibilidade parcial é o custo declarado da segurança.

Note o detalhe honesto do exemplo: tentar eleger na minoria **avança o termo** do nó isolado mesmo sem vencer. Termo não é prêmio de vencedor, é contador de tentativas. É por isso que a cura exige eleição nova: o termo velho morreu com a partição.

## 3. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/23d_consenso.c) contém o programa completo:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/23d_consenso.c -o /tmp/23d
/tmp/23d
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-sanitize-recover=all teoria/c/exemplos/23d_consenso.c -o /tmp/23d_asan
/tmp/23d_asan   # identico, sem relatorio de sanitizer
```

Saída esperada (determinística):

```text
fase1 lider+commit: N0(t=1,lider,log=1,commit=0) N1(t=1,seg,log=1,commit=0) N2(t=1,seg,log=1,commit=0)
fase2 minoria tenta eleger: sem lider (votos=1, quorum=2)
fase2 escrita da minoria: log=222 mas commit=0 (nao confirmado)
fase3 maioria confirma 333: N0(t=1,lider,log=2,commit=1) N1(t=1,seg,log=2,commit=1) N2(t=2,seg,log=2,commit=0)
fase4 pos-cura: N0(t=2,lider,log=2,commit=1) N1(t=2,seg,log=2,commit=1) N2(t=2,seg,log=2,commit=1)
23d ok: eleicao por maioria, commit por maioria, minoria nao confirma
```

Leitura das quatro fases:

1. **Eleição e commit:** N0 eleito no termo 1 com 3 votos; o comando 111 replicado aos 3 e confirmado (índice 0) nos 3.
2. **Partição, lado minoritário:** N2 sozinho tenta eleger e falha (1 voto < 2); escreve 222 no próprio log sem confirmar (`commit` continua 0). Repare no `t=2` dele na fase 3: a tentativa avançou o termo dele — derrota também conta tempo lógico.
3. **Maioria que resta:** N0 e N1 confirmam 333 sem N2. O sistema continua disponível para quem tem quorum.
4. **Cura:** eleição nova (termo 2), o log confirmado do líder (111, 333) sobrescreve a entrada não confirmada 222 em N2. O descarte é impresso pela convergência: 222 existiu e morreu por não ter sido confirmada.

### O que a memória faz aqui?

Logs limitados (16 entradas por nó, com `assert` de teto): a simulação recusa crescer além do contratado. Num sistema real o log é o disco da 23a com `fsync` antes da resposta, e o commit só avança após a maioria persistir — confirmar antes de persistir na maioria é confirmar o que um crash apaga.

## 4. Erros comuns e implicações defensivas

1. **Confirmar ao cliente antes da maioria:** a resposta "ok" vira mentira na primeira partição. Confirmação é ato público, não intenção privada do líder.
2. **Votar duas vezes no mesmo termo:** elege dois líderes e parte o log em dois. Um voto por termo por eleitor é a trava inteira da eleição.
3. **Eleger candidato desatualizado:** sem a regra "só voto em quem está pelo menos tão atualizado", um líder novo apaga escrita confirmada. Atualização antes de voto protege o commit antigo.
4. **Chamar a reconciliação da 23c de consenso:** a 23c converge descartando; esta aula impede a confirmação falsa. Nenhuma das duas impede a partição — e nenhuma dispensa quorum para confirmar.
5. **Ignorar o termo do isolado:** ele volta da partição com termo maior e depõe o líder antigo sem ter nada de novo. Termo maior sem log maior é barulho, não autoridade — por isso o voto exige atualização.
6. **Quorum par sem desempate:** com 4 nós, duas partições de 2 empatam e nada confirma. Quorum ímpar (ou testemunha) existe para que a indisponibilidade caia de um lado só.

## 5. Ponte com Python, JavaScript e as demais trilhas

O padrão é o mesmo das pontes irmãs: Go exige quorum nomeado no G6 antes de chamar qualquer coisa de "consenso"; Python cobra "o que acontece com a escrita do lado minoritário" no PY7 — esta aula é a resposta executável a essa pergunta. Em JavaScript, a eleição aparece em coordenação de abas/workers (só uma aba lidera o sync) e em backends Node com lock distribuído: quem pode confirmar sozinho, e quem precisa de dois carimbos? A pergunta muda de linguagem, não de forma.

## 6. Checklist do que esta aula garante (e do que ela não garante)

- [ ] Explico por que quaisquer duas maiorias se interceptam e por que isso impede duas confirmações conflitantes.
- [ ] Reproduzo a sequência: eleição → replicação → commit por maioria, apontando onde cada `assert` mora.
- [ ] Demonstro a minoria que não elege nem confirma, e digo onde a escrita 222 morreu.
- [ ] Descrevo o que acontece com o termo do isolado e por que a cura exige eleição nova.
- [ ] Nomeio três coisas que isto **não** é: heartbeat/timeout, membership change e prova de Raft — e onde cada uma entraria.

**Referências primárias:** [In Search of an Understandable Consensus Algorithm (Raft)](https://raft.github.io/raft.pdf) para eleição, termos e commit por maioria; [Paxos Made Simple](https://lamport.azurewebsites.net/pubs/paxos-simple.pdf) para a ideia de quorum como interseção. O [6.824 do MIT](https://pdos.csail.mit.edu/6.824/) orienta o nível dos laboratórios; nada dele foi copiado para esta aula.

*Atividades e avaliação desta aula serão publicadas ao fim do volume, junto com as demais — por decisão editorial, o conteúdo vem primeiro.*
