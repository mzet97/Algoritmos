---
layout: default
---

# Aula 23d — Quorum em Rust: a maioria decide, o borrow checker separa

**Competências SDD:** DST, CON. **Pré-requisitos:** aulas 13–14, [23b](23b_sistemas_distribuidos.md) (idempotência e log durável) e leitura da [23c de Go](../go/23c_quorum_e_reparo.md) é recomendada. Esta ponte fecha o R6 na parte de ordem: relógios vetoriais e quorum de escrita/leitura com reparo, em Rust puro sem crates.

**Objetivo observável:** ao terminar, você deverá escrever com quorum (W=2 de 3), ler com quorum declarando conflito em vez de servir o obsoleto, reparar a atrasada — e explicar por que o reparo mora numa segunda passada, apontando o erro de borrow que a primeira tentativa comete.

**Motivação:** a 23b prova distribuição com processos reais; esta prova a regra de decisão com simulação determinística. W+R > N é a mesma interseção em toda trilha: a escrita confirmada tocou 2 réplicas, a leitura consulta 2, e 1 réplica em comum carrega a verdade.

## 1. Contrato antes do código

- **Escrita com quorum:** coordenador incrementa o próprio componente, replica para as vivas e confirma com 2 acks. Sem maioria, sem confirmação.
- **Leitura com quorum:** 2 respostas mínimas; a entrada que domina as demais vence; empate de vetores é versão replicada (não conflito); sem dominância, `Conflito` em vez de valor obsoleto.
- **Reparo em duas passadas:** a primeira só lê (`&`); a segunda escreve (`&mut`). O borrow checker proíbe reparar durante a coleta — e ele tem razão: mutar o mapa enquanto se decide a vencedora é decidir com dado em movimento.
- **Partição:** lado de 1 não escreve nem lê por quorum. Indisponibilidade declarada.
- **Fora do contrato:** eleição de líder e log durável (a 23b cobre); aqui o coordenador é fixo e a memória é o estado.

## 2. O que Rust cobra neste desenho

Duas passadas não são estilo — são a única forma que compila: `replicas.iter()` empresta imutavelmente para coletar, e `replicas.iter_mut()` só entra depois que o empréstimo morreu. Em C e Go, reparar durante a coleta compila e corrompe a decisão em silêncio se a ordem mudar; aqui, o compilador transforma a armadilha em erro de compilação com número da linha. O `match` em `Leitura` (enum de três variantes) obriga o chamador a tratar `Conflito` e `SemQuorum` — não há "valor com cara de verdade" escorrendo para o cliente.

## 3. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/23d_consenso.rs) contém a simulação, só biblioteca padrão:

```bash
rustc --edition 2024 -O -D warnings teoria/rust/exemplos/23d_consenso.rs -o /tmp/23d_rs
/tmp/23d_rs
```

Com clippy (exigido para ponte: limpo com `-D warnings`):

```bash
cargo clippy -- -D warnings   # no projeto temporário da verificação
```

Saída esperada (determinística):

```text
fase1 quorum limpo: k=111
fase2 minoria sem quorum: escrita e leitura recusadas
fase3 pos-cura: k=222, atrasada reparada na leitura
23d ok: escrita exige maioria, leitura encontra, reparo converge
```

## 4. Erros comuns e implicações defensivas

1. **Reparar durante a coleta:** em Rust não compila; em outra linguagem, decide com dado em movimento. A segunda passada não é cerimonial.
2. **Acusar igualdade de conflito:** vetores idênticos são a mesma versão replicada. O `continue` na igualdade veio da correção da [23c de C++](../cpp/23c.md) — mesma classe de bug, terceira trilha.
3. **Confirmar com W=1:** a escrita mora onde a leitura não olha. Rapidez comprada com verdade alheia.
4. **Tratar `Conflito` como erro genérico:** conflito pede reconciliação (regra publicada), não retry — repetir a leitura devolve o mesmo conflito.
5. **Chamar isto de consenso:** sem eleição, o coordenador fixo é ponto único. Quorum aqui, commit com líder na [23d de C](../c/23d_consenso_e_quorum.md).

## 5. Ponte com as demais trilhas

C confirma por maioria com termo e eleição; C++ declara conflito no quorum e confirma na 23f; Go repara na leitura com `-race` limpo. Em JavaScript, o análogo são CRDTs e sync offline-first — a réplica isolada desta aula é o cliente sem rede.

## 6. Checklist do que esta aula garante

- [ ] Explico W+R > N desenhando a interseção e digo onde cada ack é contado.
- [ ] Distingo igualdade (mesma versão) de concorrência (conflito), com um par de cada.
- [ ] Aponto as duas passadas e o erro de borrow que uma passada só cometeria.
- [ ] Demonstro minoria sem quorum e cura com reparo.
- [ ] Nomeio o que falta: eleição, log durável e teste com processos reais (a 23b).

**Referências primárias:** [Dynamo (Amazon)](https://www.amazon.science/publications/dynamo-amazons-highly-available-key-value-store) para W/R/N e reparo de leitura; [Raft](https://raft.github.io/raft.pdf) para o que quorum sem eleição não entrega.

*Atividades e avaliação desta aula serão publicadas ao fim do volume, junto com as demais — por decisão editorial, o conteúdo vem primeiro.*
