---
layout: default
---

# Aula 14b — Ordem de memória, backpressure e cancelamento em Rust

## Nesta aula

Você vai aprender a explicar ordem de memória (o que `Mutex`/atômicos garantem e não garantem), implementar backpressure com canal limitado, e tratar cancelamento sem vazar tarefa nem perder dado.

Você vai demonstrar publicação de dados por atômicos com `Release`/`Acquire` (10 mil voltas verificadas), bloquear um produtor num canal limitado de 4 com o consumidor dormindo (backpressure medida em ~300 ms) e parar um worker por cancelamento cooperativo com flag atômica — sem matar a thread.

o R5 pedia modelo de memória com ordenação atômica declarada, backpressure e cancelamento. A aula 14 apresentou os primitivos; aqui cada um vira comportamento medido. O ponto de Rust: data race é erro de compilação, **mas ordem fraca demais compila feliz** — `Relaxed` onde devia ser `Release` não é detectado por ninguém nesta aula, só pela sua prova. É por isso que a ordenação se escreve com justificativa, não por hábito.

## Contrato e exemplo

O [arquivo-fonte](exemplos/14b_ordem.rs) contém o programa completo:

```bash
rustc --edition 2024 -O -D warnings teoria/rust/exemplos/14b_ordem.rs -o /tmp/14b
/tmp/14b
# mensagem Release/Acquire ok (10000 voltas)
# backpressure: 5o envio esperou 300ms (canal de 4, consumidor dormindo)
# cancelamento cooperativo ok (3.3M voltas ate ver a flag)
# 14b ok: ...
```

As três garantias, uma a uma:

1. **Publicação:** o produtor grava `DADO` e depois `PRONTO.store(true, Release)`; o consumidor gira em `PRONTO.load(Acquire)` e só então lê `DADO`. O par Release/Acquire cria o happens-before: quem vê a bandeira vê o dado. Trocar os dois por `Relaxed` compila igual e não garante nada — em x86 costuma "funcionar", que é o pior tipo de errado (na 14a de C o mesmo par aparece; a prova é do modelo, não da máquina).
2. **Backpressure:** `sync_channel(4)` é a fila com teto. Com o consumidor dormindo 300 ms, o quinto `send` bloqueia até haver espaço: fila limitada transforma produtor rápido em produtor no ritmo do consumidor, que é o comportamento que protege memória. Canal ilimitado (`channel`) trocaria esse bloqueio por crescimento de fila até o OOM — a aula mede o bloqueio para ele ser uma escolha, não um susto.
3. **Cancelamento cooperativo:** o worker consulta `PARAR` a cada volta e sai sozinho; ninguém "mata" thread em Rust (não existe API segura para isso, de propósito). Quem precisa parar no meio de um bloqueio usa timeout de canal ou I/O com prazo — cancelamento é protocolo, não sinal.

### O grafo happens-before das 10 mil voltas (desenhado por extenso)

Cada volta tem quatro nós: `grava DADO` → `store PRONTO(Release)` → `load PRONTO(Acquire)` → `lê DADO`. A aresta do meio (Release → Acquire) é a única que cruza threads — e é ela que carrega as outras duas: sem ela, `grava` e `lê` são eventos soltos em threads distintas, sem ordem, e o programa tem data race por definição do modelo (dois acessos, um escrita, sem happens-between). O `join` no fim adiciona a aresta final: tudo que o worker fez precede o retorno do `join` — sem ele, a main poderia ler `DADO` depois das 10 mil voltas e ainda assim correr, porque término de thread sem `join` não publica nada. Trocar o par por `Relaxed` apaga a aresta do meio mantendo os nós: o programa executa igual em x86 (TSO segura as aparências) e o teste passa — e é exatamente esse o perigo que a motivação denuncia: teste verde sobre grafo quebrado. O exercício 4 manda desenhar o grafo completo incluindo o `join`; quem desenha vê que a prova tem 10 001 arestas inter-thread, não "umas sincronizações".

### Leitura guiada: três linhas, três mecanismos

A primeira (`mensagem Release/Acquire ok (10000 voltas)`) prova a aresta: 10 mil publicações, cada uma lida íntegra do outro lado do par — e o número de voltas importa porque corrida intermitente se esconde em poucas amostras. A segunda (`backpressure: 5o envio esperou 300ms`) prova o teto com o relógio: o quinto `send` esperou exatamente o sono do consumidor — nem menos (teto furado), nem para sempre (deadlock). O `300ms` é medida, não veredito; o veredito é "esperou". A terceira (`cancelamento cooperativo ok (3.3M voltas ate ver a flag)`) prova o protocolo com latência declarada: o worker viu a flag depois de 3,3 milhões de voltas — parar "rápido" não é requisito, parar sozinho é. Três linhas, três provas, três instrumentos distintos (assert de dado, relógio, contagem de voltas).

## Erros comuns

1. **`SeqCst` em tudo por medo:** funciona e ensina nada; cada `Relaxed`/`Release`/`Acquire` deste arquivo tem uma frase de justificativa, e é ela que se revisa.
2. **Spin sem `spin_loop`:** girar em `load` puro queima pipeline e pode ser otimizado para um laço infinito pelo compilador em outras linguagens; `std::hint::spin_loop` é a pausa arquitetural e a âncora contra a otimização.
3. **Canal ilimitado "para não travar":** o travamento vira acúmulo. Backpressure é o sistema dizendo a verdade sobre velocidades diferentes.
4. **Esperar que `Drop` do canal cancele o worker:** fechar o canal acorda quem espera **no canal**; quem computa nem fica sabendo. Flag consultada ou mensagem de parada: um dos dois, explícito.
5. **Ler `DADO` depois do `join` sem par Release/Acquire:** o `join` publica tudo que o worker fez — mas só o que o worker fez antes de terminar. Leitura escrita DEPOIS do join (ou sem join) não está coberta por aresta nenhuma; `join` não é cerca universal, é aresta de término.
6. **Medir backpressure com consumidor acordado:** com o consumidor drenando, o quinto `send` nunca bloqueia e a aula "prova" que teto não segura. O sono de 300 ms é o instrumento: sem carga que exceda o teto, teto não é testado.

## Exercícios, pistas e solução

Exercícios em [pistas](exercicios/14b_pistas.md) e [soluções](exercicios/14b_solucoes.md): (1) trocar `Release`/`Acquire` por `Relaxed` e explicar por que o teste continua passando em x86 e o que ele deixaria de provar; (2) medir a vazão do canal com teto 0 (rendezvous), 4 e 1024; (3) fazer o worker parar no meio de um `recv` usando `recv_timeout` como cancelamento; (4) desenhar o grafo happens-before completo das 10 mil voltas, incluindo o `join`.

**Checklist:** cada ordenação com justificativa escrita; backpressure medida, não afirmada; cancelamento sem morte de thread; `-D warnings` limpo e nenhum `sleep` usado como sincronização (só como carga).

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

1. *The Rustonomicon* ([doc.rust-lang.org/nomicon](https://doc.rust-lang.org/nomicon)); 2. Documentação do Tokio ([tokio.rs](https://tokio.rs)); 3. *The Rust Performance Book* ([nnethercote.github.io/perf-book](https://nnethercote.github.io/perf-book)).
