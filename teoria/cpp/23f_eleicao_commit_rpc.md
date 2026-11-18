---
layout: default
---

# Aula 23f - Eleição, commit e RPC idempotente: a escrita que conta

**Objetivo:** eleger um líder por maioria, confirmar comandos só na maioria, descartar a escrita solitária na cura e servir repetições do cache — sobre a base das aulas 23b (idempotência) e 23c (vetor e quorum na leitura). Pré-requisitos: aulas 23, 23a–23e. Linux ou WSL2. Sem QEMU e sem GPU.

Ao terminar, você deverá eleger e depor líderes, apontar onde o commit acontece e explicar por que a repetição nunca reexecuta — inclusive quando a primeira resposta foi erro.

Esta aula fecha o módulo CPP-8 na parte de escrita: a 23c protegeu a leitura com quorum; esta confirma a escrita com maioria e nomeia o RPC idempotente que a serve. O que falta depois dela: TLS (laboratório opcional) e o kernel das aulas 33–40.

**Como ler esta aula (tres niveis):** *fundamental* -- maioria confirma, minoria nao confirma; *intermediario* -- as quatro fases e o termo; *aprofundamento* -- heartbeat e o Raft que isto nao e.

## 1. Contrato antes do código

- **Eleição:** candidato vota em si, pede voto uma vez por termo; eleitor vota só em candidato pelo menos tão atualizado (tamanho do log como relógio). Maioria (2 de 3) elege; minoria de 1 nunca elege — um voto não é quorum.
- **Replicação e commit:** o líder anexa no próprio log, copia para os vivos do subconjunto e confirma só com maioria de acks. `commit` é índice, não promessa: aponta a última entrada que a maioria tem.
- **Partição:** o lado de 1 escreve sozinho sem confirmar (o `commit` não anda) e a cura com termo novo sobrescreve o não confirmado com o log do líder. Descarte nomeado, não silencioso.
- **RPC idempotente:** `id` repetido devolve o gravado com marca de repetição, sem reexecutar; erro também é gravado — reavaliar o erro seria reabrir decisão tomada; sem `id`, recusa.
- **Fora do contrato:** heartbeat/timeout (sem relógio aqui), log compaction e membership change. O coordenador da simulação é fixo por partição; eleição real o escolheria.

## 2. Por que o erro também se memoriza

Porque o cliente repete o erro tanto quanto o acerto: timeout não distingue "não executou" de "resposta perdida". Se o servidor reexecutar método inválido a cada repetição, um método com efeito parcial pode completar na segunda tentativa o que recusou na primeira — e a recusa virou mentira retroativa. Erro gravado congela a decisão: o pedido morreu uma vez.

## 3. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/23f_commit.cpp) contém a simulação (`g++` e `clang++`, `-Werror` nos dois):

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror teoria/cpp/exemplos/23f_commit.cpp -o srv23f
./srv23f
clang++ -std=c++20 -Wall -Wextra -Wpedantic -Werror teoria/cpp/exemplos/23f_commit.cpp -o srv23f_clang
./srv23f_clang   # idêntico
```

Com sanitizers (exigido para ponte: ASan/UBSan limpos):

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-omit-frame-pointer teoria/cpp/exemplos/23f_commit.cpp -o srv23f_asan
./srv23f_asan
```

Saída esperada (determinística):

```text
fase1 lider eleito, op-1 confirmada por maioria
fase2 minoria sem lider; escrita solitaria nao confirmada
fase3 maioria confirma op-2 sem o isolado
fase4 pos-cura: op-solitaria descartada, op-2 confirmada
rpc: repeticao do cache, erro memorizado, sem id recusado
23f ok: eleicao, commit por maioria, RPC idempotente
```

Dois erros reais foram pegos no caminho, ambos antes de qualquer execução: sintaxe de outra linguagem no `assert` e chamada a método inexistente (restos de rascunho), e o `#include <vector>` faltando que o `-Werror` transformou em cascata instrutiva — incluir o que usa, sempre.

### Leitura guiada: seis linhas, uma história de maioria

Cada fase prova uma propriedade diferente, e a ordem é a demonstração: fase 1 estabelece o normal (líder + maioria confirma) para que as fases seguintes tenham contra o que contrastar; fase 2 isola a minoria (1 nó escreve, nada confirma — o `commit` parado é a prova de que confirmação é contagem, não vontade); fase 3 mostra a maioria vivendo sem o isolado (quorum é sobre quem resta, não sobre quem falta); fase 4 executa a cura (termo novo, log do líder sobrescreve o não confirmado — descarte com testemunha, a `op-2` confirmada); a linha do RPC prova que repetição e erro moram no cache, não na reexecução; o veredito fecha o par eleição+RPC. Se a fase 4 viesse antes da 2, o descarte pareceria arbitrário — a ordem fase-a-fase é o argumento de que maioria confirma, minoria sugere e cura decide.

## 4. Erros comuns e implicações defensivas

1. **Confirmar sem maioria:** resposta "ok" com 1 ack é a confirmação falsa da 23d de C. Commit é contagem, não intenção.
2. **Eleger desatualizado:** sem a regra de atualização, o líder novo apaga escrita confirmada. O voto protege o commit antigo.
3. **Reexecutar na repetição:** retry sem `id` é duplicação com passos extras; com `id` e sem cache, é duplicação com placa.
4. **Reavaliar o erro:** cada repetição redecide o indecidível. Erro gravado é decisão, não palpite.
5. **Chamar isto de Raft:** sem heartbeat, sem persistência em disco e sem compaction, é o núcleo didático. O nome certo evita a confiança errada.
6. **Votar sem olhar o log:** eleitor que vota no primeiro candidato com termo maior elege desatualizado — e o líder novo apaga confirmada. A regra de atualização (tamanho do log como relógio) é o que faz o voto proteger o commit antigo em vez de só contar cabeças.
7. **Cachear só o acerto:** se o erro não é gravado, o retry do cliente reexecuta método inválido até ele "passar" por efeito parcial — e a recusa vira mentira retroativa. O cache guarda decisões, não sucessos; erro é decisão.

## 5. Ponte com as demais trilhas

C elege e confirma na 23d e numera o RPC na 23e; Go exige quorum W+R na 23c; Rust audita o dono de cada byte que cruza fronteira. Em JavaScript, o análogo é o `Idempotency-Key` com confirmação — mesma tabela, outro transporte.


### Pesos da avaliacao

| Criterio | Peso |
|---|---|
| Eleicao + commit executados com fases explicadas | 35% |
| Idempotencia do RPC com erro memorizado | 25% |
| Diagnostico (prever termo/voto/commit antes de cada fase) | 25% |
| Documentacao (comandos, saidas) | 15% |

**Referencias primarias:** [Raft](https://raft.github.io/raft.pdf) para o que falta (heartbeat, persistencia).

## 6. Critério de aprovação e checklist

- [ ] Elejo e deponho líderes explicando cada voto, termo e recusa.
- [ ] Aponto onde o commit anda e onde ele para — com o número de acks.
- [ ] Demonstro a escrita solitária descartada e digo onde ela morreu.
- [ ] Mostro repetição e erro servidos do cache, com o contador de execuções parado.
- [ ] Nomeio o que falta: heartbeat, persistência, compaction e TLS.

**Referências primárias:** [Raft](https://raft.github.io/raft.pdf) para eleição, termos e commit; especificação de [Idempotency-Key do Stripe](https://docs.stripe.com/api/idempotent_requests) para o RPC. Nenhum destes textos foi copiado — o exemplo é simulação própria sobre as regras deles.

## 7. Exercícios obrigatórios

1. (Previsão) Empurre `"op-fantasma"` direto no `log` do líder (sem `replicar`): preveja `log.size()` e `commit`. Rode e explique por que presença no log não é confirmação — e qual invariante o `commit` protege.
2. (Diagnóstico) Baixe `QUORUM` para 1 e eleja na minoria `{2}`: ela elege? A escrita solitária confirma? Rode e explique por que "qualquer um" confirma a solidão — e o que a cura faria com duas confirmações conflitantes.
3. (Projeto) Ponha teto no cache RPC (2 ids, despeja o mais antigo): reenvie `req-1` após `req-3` expulsá-lo e prove que reexecuta (`execucoes` anda). Explique o preço do teto: idempotência com memória limitada dura até o despejo.
4. (Previsão) Replique incluindo o morto (`vivo = false`) na lista: ele é pulado? O commit avança sem ele? Rode e explique com o `log` de N2 após a fase 3 — desatualizado até a cura — e por que morto não vota nem confirma, mas volta a seguir.

Use [pistas graduais](exercicios/23f_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/23f_solucoes.md) depois de registrar sua própria explicação.

---

### Para se aprofundar

1. Martin Kleppmann, *Designing Data-Intensive Applications* (O'Reilly, ISBN 978-1449373320), caps. 8-9; 2. *Raft paper* (Ongaro e Ousterhout, [raft.github.io](https://raft.github.io/)); 3. W. Richard Stevens et al., *UNIX Network Programming, vol. 1*.
