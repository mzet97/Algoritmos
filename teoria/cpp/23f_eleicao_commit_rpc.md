---
layout: default
---

# Aula 23f - Eleição, commit e RPC idempotente: a escrita que conta

**Objetivo:** eleger um líder por maioria, confirmar comandos só na maioria, descartar a escrita solitária na cura e servir repetições do cache — sobre a base das aulas 23b (idempotência) e 23c (vetor e quorum na leitura). Pré-requisitos: aulas 23, 23a–23e. Linux ou WSL2. Sem QEMU e sem GPU.

Ao terminar, você deverá eleger e depor líderes, apontar onde o commit acontece e explicar por que a repetição nunca reexecuta — inclusive quando a primeira resposta foi erro.

Esta aula fecha o módulo CPP-8 na parte de escrita: a 23c protegeu a leitura com quorum; esta confirma a escrita com maioria e nomeia o RPC idempotente que a serve. O que falta depois dela: TLS (laboratório opcional) e o kernel das aulas 33–40.

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

## 4. Erros comuns e implicações defensivas

1. **Confirmar sem maioria:** resposta "ok" com 1 ack é a confirmação falsa da 23d de C. Commit é contagem, não intenção.
2. **Eleger desatualizado:** sem a regra de atualização, o líder novo apaga escrita confirmada. O voto protege o commit antigo.
3. **Reexecutar na repetição:** retry sem `id` é duplicação com passos extras; com `id` e sem cache, é duplicação com placa.
4. **Reavaliar o erro:** cada repetição redecide o indecidível. Erro gravado é decisão, não palpite.
5. **Chamar isto de Raft:** sem heartbeat, sem persistência em disco e sem compaction, é o núcleo didático. O nome certo evita a confiança errada.

## 5. Ponte com as demais trilhas

C elege e confirma na 23d e numera o RPC na 23e; Go exige quorum W+R na 23c; Rust audita o dono de cada byte que cruza fronteira. Em JavaScript, o análogo é o `Idempotency-Key` com confirmação — mesma tabela, outro transporte.

## 6. Critério de aprovação e checklist

- [ ] Elejo e deponho líderes explicando cada voto, termo e recusa.
- [ ] Aponto onde o commit anda e onde ele para — com o número de acks.
- [ ] Demonstro a escrita solitária descartada e digo onde ela morreu.
- [ ] Mostro repetição e erro servidos do cache, com o contador de execuções parado.
- [ ] Nomeio o que falta: heartbeat, persistência, compaction e TLS.

**Referências primárias:** [Raft](https://raft.github.io/raft.pdf) para eleição, termos e commit; especificação de [Idempotency-Key do Stripe](https://docs.stripe.com/api/idempotent_requests) para o RPC. Nenhum destes textos foi copiado — o exemplo é simulação própria sobre as regras deles.

*Atividades e avaliação desta aula serão publicadas ao fim do volume, junto com as demais — por decisão editorial, o conteúdo vem primeiro.*
