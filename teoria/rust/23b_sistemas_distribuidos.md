---
layout: default
---

# Aula 23b — Sistemas distribuídos em Rust: o mesmo contrato, outra linguagem

O mesmo contrato distribuído da trilha de C — framing, falha parcial, repetição — reescrito onde o borrow checker separa dono de emprestado. Outra linguagem, as mesmas leis da rede.

## Nesta aula

Você vai aprender a enunciar o contrato de um sistema distribuído (o que cada nó garante), distinguir falha de processo de partição, e explicar quorum em termos de maioria.

Você vai ao terminar, você deverá executar dois nós chave-valor Rust (processos reais via `current_exe`) que persistem cada escrita com `sync_all` antes de responder, recusam duplicar efeito em reenvio com o mesmo `id`, e sobrevivem — com comportamento explicado — a duplicata com valor diferente, pedido malformado, `kill` do processo, cauda de log rasgada, reinício com replay e atraso injetado de 400 ms.

HTTP e gRPC das aulas 20–23 provam que um serviço responde; não provam distribuição. O R6 do mapa pedia exatamente o que faltava: falha parcial, repetição, persistência e recuperação executadas, com a garantia nomeada e a lacuna declarada. Rust muda a implementação (ownership do estado compartilhado, `Mutex` envenenado, `Result` em cada borda) — o contrato distribuído, não.

## 1. Contrato antes do código

Protocolo por linhas em loopback (o mesmo da 23a de C, deliberadamente): `PUT <id> <chave> <valor> [F]`, `GET <chave>`; respostas `OK`, `OK ... REPETIDO`, `NAO_ENCONTRADO`, `ERR`.

- **Persistir antes de responder:** `write_all` + `sync_all` no log, dentro do `Mutex`, antes de aplicar em memória e de responder. Responder antes do disco é mentir ao cliente na janela da queda.
- **Idempotência:** `por_id` reconstruído pelo replay; duplicata devolve a primeira escrita marcada `REPETIDO` e não reencaminha. O `id` nasce no cliente e atravessa os retries.
- **Encaminhar fora do `Mutex`, antes de responder, com a marca `F`:** quem recebe `F` não reencaminha; a trava nunca espera rede.
- **Fora do contrato:** consenso, quorum, partição bilateral resolvida, TLS/autenticação. Loopback local apenas. A comparação honesta com relógios e anti-entropia está na [23c de C](../c/23c_relogios_e_particao.md).

## 2. O que Rust cobra neste desenho

Três pontos onde a linguagem aparece de verdade, não como sintaxe:

1. **`Mutex` envenenado é um alarme, não um detalhe:** se uma thread entra em pânico segurando o `Mutex`, o `lock().unwrap()` seguinte também entra em pânico — o estado pode estar no meio de "persistiu mas não aplicou". Este desenho mantém a seção crítica curta e sem pânico esperado; um serviço real decidiria explicitamente entre propagar o envenenamento ou reconstruir o estado pelo replay (que aqui existe exatamente para isso).
2. **`Result` em cada borda de I/O:** ler linha com teto (`take(LINHA_MAX + 1)`) transforma "linha infinita" em erro tipado que fecha só aquela conexão. O servidor não tem caminho de `panic!` por entrada hostil: parsing devolve `ERR`, I/O devolve `Err`, e os dois estão escritos no código, não na esperança.
3. **Ownership do log:** o `File` mora dentro do estado travado; ninguém escreve fora da ordem validar → persistir → aplicar porque ninguém mais alcança o arquivo. Em C, essa ordem era disciplina comentada; aqui, o grafo de ownership a impõe estruturalmente — e o preço é a contenção de um `Mutex` grosso, declarada.

## 3. Exemplo completo e executável

O [arquivo-fonte](exemplos/23b_distribuido.rs) é o programa completo, só biblioteca padrão:

```bash
rustc --edition 2024 -D warnings teoria/rust/exemplos/23b_distribuido.rs -o /tmp/r23b
/tmp/r23b checar
```

Saída esperada:

```text
ok 1 - PUT inicial OK
ok 2 - replica recebe
ok 3 - duplicata vale a primeira (REPETIDO)
ok 4 - malformado recebe ERR
ok 5 - escrita com replica caida (degradado)
ok 6 - replay ignora cauda rasgada
ok 7 - escrita da queda ausente na replica
ok 8 - reenvio dirigido converge
ok 9 - atraso de 400 ms atrasa sem travar
ok 10 - atrasada chega a replica
DISTRIBUIDO_OK checks=10
```

O roteiro mata processos filhos com `Child::kill` (SIGKILL no Unix), rasga a cauda do log com bytes sem `\n` e mede o atraso com `Instant`. Cada `ok` corresponde a uma cláusula do contrato; rodar fora do `checar` (`no <porta> <log> <par> <atraso_ms>`) sobe um nó manual para explorar com `nc`.

## 4. Erros comuns e implicações defensivas

1. **`lock().unwrap()` sem pensar no envenenamento:** pânico de uma conexão não pode virar pânico do nó inteiro sem uma decisão escrita. Ou a seção crítica é à prova de pânico, ou a recuperação é pelo replay — os dois, documentados.
2. **Ler sem teto:** `read_line` direto em `String` aceita a linha do tamanho que o atacante quiser. Teto com `take`, erro fecha a conexão.
3. **Encaminhar segurando o `Mutex`:** par lento serializa o nó; a trava protege o estado, nunca espera a rede.
4. **Dedup só em memória:** reinício esqueceria os `id`s; aqui eles renascem do log no replay.
5. **Tratar `kill` de processo filho como falha do teste:** a queda **é** o teste. Serviços que só funcionam sem queda são centralizados com passos extras (23a de Python).

## 5. Ponte com as outras trilhas

Mesmo protocolo, mesmo roteiro, quatro linguagens: C (23a, processos e `fsync`), C++ (23b, log com selo de cauda), Go (23a, framing binário e `-race`) e Python (23a, dois processos). Rust entra por último de propósito: depois que o contrato está claro, a pergunta vira o que o sistema de tipos compra (ownership do log, `Result` nas bordas) e o que ele não compra (consenso continua sendo outro curso).

## 6. Exercícios obrigatórios

1. Faça uma thread de conexão entrar em pânico de propósito durante um `PUT` e observe o envenenamento do `Mutex` nas conexões seguintes. Implemente a recuperação pelo replay e mostre o nó voltando a responder.
2. Adicione teto de conexões simultâneas com `Semaphore` artesanal (`Mutex` + `Condvar`) e mostre o cliente excedente esperando em vez de derrubar o nó.
3. Troque o log por registros com comprimento e CRC (como o selo da 23b de C++) e mostre uma cauda rasgada no meio do registro sendo descartada sem perder os registros anteriores.
4. Escreva o parágrafo de ameaças deste nó em loopback: quem pode conectar, o que um `PUT` infinito custa, e o que muda antes de sair de `127.0.0.1`.

Use [pistas graduais](exercicios/23b_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/23b_solucoes.md) depois de registrar sua própria explicação.

## 7. Ao final, você consegue

Você concluiu a aula se consegue, sem olhar a solução:

- ordenar validar → persistir (`sync_all`) → aplicar → encaminhar → responder, e dizer o que cada inversão quebra;
- explicar o envenenamento do `Mutex` e a sua política para ele;
- demonstrar duplicata, malformado, queda, cauda rasgada, replay, reenvio e atraso pelo roteiro;
- apontar onde o ownership substitui disciplina manual — e onde não substitui;
- compilar com `-D warnings`, rodar os 10 checks e declarar as lacunas (consenso, partição resolvida, segurança fora de loopback).

**Próxima aula:** 24 — desempenho, onde o `Mutex` grosso desta aula vira hipótese a medir.

---

### Como esta aula será avaliada

| Critério | Peso |
| -------- | ---: |
| Conceitos e vocabulário (nível 1) | 20% |
| Previsão de comportamento e diagnóstico de erros (nível 2) | 40% |
| Projeto e justificativa de decisões (nível 3) | 30% |
| Registro de comandos, saídas e interpretação | 10% |

**Aprovação:** nota mínima 7,0; pelo menos 60% dos pontos devem vir de previsão e diagnóstico (níveis 2 e 3), não apenas de código que compila.

**Para se aprofundar:**

1. *Designing Data-Intensive Applications*, Martin Kleppmann (O'Reilly, ISBN 978-1449373320), caps. 8–9; 2. Documentação do Tokio ([tokio.rs](https://tokio.rs)).
