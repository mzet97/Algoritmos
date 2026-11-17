---
layout: default
---

# Aula 14b — Ordem de memória, backpressure e cancelamento em Rust

**Competências SDD:** CON, MEM. **Pré-requisitos:** aulas 13–14, [14a](14a_framing_e_concorrencia.md) e 17a de C++ (o mesmo modelo de memória, outra sintaxe). Você precisa saber `std::thread`, `mpsc` e o vocabulário happens-before.

**Objetivo observável:** demonstrar publicação de dados por atômicos com `Release`/`Acquire` (10 mil voltas verificadas), bloquear um produtor num canal limitado de 4 com o consumidor dormindo (backpressure medida em ~300 ms) e parar um worker por cancelamento cooperativo com flag atômica — sem matar a thread.

**Motivação:** o R5 pedia modelo de memória com ordenação atômica declarada, backpressure e cancelamento. A aula 14 apresentou os primitivos; aqui cada um vira comportamento medido. O ponto de Rust: data race é erro de compilação, **mas ordem fraca demais compila feliz** — `Relaxed` onde devia ser `Release` não é detectado por ninguém nesta aula, só pela sua prova. É por isso que a ordenação se escreve com justificativa, não por hábito.

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

## Erros comuns

1. **`SeqCst` em tudo por medo:** funciona e ensina nada; cada `Relaxed`/`Release`/`Acquire` deste arquivo tem uma frase de justificativa, e é ela que se revisa.
2. **Spin sem `spin_loop`:** girar em `load` puro queima pipeline e pode ser otimizado para um laço infinito pelo compilador em outras linguagens; `std::hint::spin_loop` é a pausa arquitetural e a âncora contra a otimização.
3. **Canal ilimitado "para não travar":** o travamento vira acúmulo. Backpressure é o sistema dizendo a verdade sobre velocidades diferentes.
4. **Esperar que `Drop` do canal cancele o worker:** fechar o canal acorda quem espera **no canal**; quem computa nem fica sabendo. Flag consultada ou mensagem de parada: um dos dois, explícito.

## Exercícios, pistas e solução

Exercícios em [pistas](exercicios/14b_pistas.md) e [soluções](exercicios/14b_solucoes.md): (1) trocar `Release`/`Acquire` por `Relaxed` e explicar por que o teste continua passando em x86 e o que ele deixaria de provar; (2) medir a vazão do canal com teto 0 (rendezvous), 4 e 1024; (3) fazer o worker parar no meio de um `recv` usando `recv_timeout` como cancelamento; (4) desenhar o grafo happens-before completo das 10 mil voltas, incluindo o `join`.

**Checklist:** cada ordenação com justificativa escrita; backpressure medida, não afirmada; cancelamento sem morte de thread; `-D warnings` limpo e nenhum `sleep` usado como sincronização (só como carga).
