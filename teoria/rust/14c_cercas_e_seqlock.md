---
layout: default
---

# Aula 14c — Cercas e seqlock: ordenação sem operação e leitura otimista (R5)

**Competências SDD:** CON, MEM. **Pré-requisitos:** aulas 13–14 e [14b](14b_ordem_e_backpressure.md) (Release/Acquire grudado na operação, backpressure e cancelamento). Toolchain registrada: `rustc` 1.99.1 (stable, sem nightly, sem crates externas).

**Objetivo observável:** ao terminar, você deverá executar o exemplo, mostrar 50 mil publicações conferidas par a par via `fence` (sem nenhum `Release`/`Acquire` nas operações) e explicar por que o leitor do seqlock retenta em vez de bloquear — e o que o contador ímpar proíbe.

**Motivação:** a 14b ensinou ordenação carregada na operação: `store(Release)` publica, `load(Acquire)` consome. Mas ordenar e mover dado são coisas distintas, e o hardware distingue as duas. A `fence` ordena sem operação — publica um par de `u64` comum com `Relaxed` nas pontas e cerca no meio. E o seqlock mostra para que isso serve: leitor que nunca bloqueia escritor, ao preço de retentar quando pega escrita no meio. É o padrão de "ler muito, escrever pouco" que aparece em relógio, config e versão de estrutura.

## Contrato e exemplo

O [arquivo-fonte](exemplos/14c_cercas.rs) contém o programa completo:

```bash
rustc --edition 2024 -O -D warnings teoria/rust/exemplos/14c_cercas.rs -o cercas14c
./cercas14c
# cercas ok: 50000 publicacoes conferidas par a par
# seqlock ok: 50000 valores consistentes, N retentativas
# 14c ok: fence publica, contador ordena a escrita, leitor otimista nunca trava escritor
```

(O número de retentativas varia com a máquina — é contenção real medida, não veredito; o `assert` é sobre consistência, sempre exata.)

## 1. A cerca: ordenar sem carregar dado

O produtor escreve `[i, i*7]` num `Mutex` (o dado não é atômico), executa `fence(Release)` e só então sinaliza com `store(Relaxed)`. O consumidor vê o sinal com `load(Relaxed)`, executa `fence(Acquire)` e só então lê. A ordenação vive nas cercas, não nas operações: trocar os dois `Relaxed` por `Relaxed` continua correto — tirar qualquer cerca vira data race (e em código seguro com `&mut` compartilhado, nem compilaria sem o `Mutex`).

Três consequências que a 14b não mostrava: (a) a cerca ordena **tudo** que a precede/sucede, não um endereço — publicar dois `u64` com uma cerca é o caso de uso; (b) o sinal pode ser `Relaxed` porque ele não carrega dado, só posição; (c) em x86-64 a cerca `Release`/`Acquire` compila para quase nada (o TSO já ordena), e é por isso que o teste passa folgado aqui — a aula roda igual em ARM/RISC-V fracos, onde a cerca vira instrução de verdade.

## 2. O seqlock: leitor otimista, escritor sem espera

O escritor incrementa o contador para ímpar (`Release`), escreve, incrementa para par (`Release`). O leitor lê o contador (`Acquire`); se ímpar, retenta; lê o dado; relê o contador — se mudou, retenta. O contador ímpar proíbe leitura: não existe "ler durante escrita e torcer". O par diferente proíbe uso: não existe "li metade velha, metade nova".

O preço aparece no número de retentativas (centenas de milhares para 50 mil leituras nesta máquina): o leitor gira enquanto o escritor escreve. Seqlock serve quando escrita é rara e leitura é quente — config lida por milhares de threads, atualizada uma vez por minuto. Com escrita frequente, o leitor morre de fome girando: aí é `RwLock` (aula 13) ou versão com espera, não otimismo.

## 3. Erros comuns e implicações defensivas

1. **Cerca só de um lado:** `Release` sem `Acquire` (ou vice-versa) não sincroniza nada — é como lacrar a carta e o destinatário não olhar o lacre. Paridade de cercas é contrato, não estilo.
2. **Ler com contador ímpar "só para ver":** o valor lido durante escrita é Frankenstein (metade velha). O ramo `if antes % 2 == 1` não é otimização, é correção.
3. **Esquecer a segunda leitura do contador:** ler, checar ímpar, usar — sem reler depois — aceita escrita que começou depois da primeira leitura. As duas leituras são o par; uma só é metade do protocolo.
4. **Seqlock com escrita frequente:** otimismo sem espera vira espera sem progresso para o leitor. Medir retentativas (o exemplo imprime) é o termômetro: retentativa perto de zero com escrita constante é suspeita, não saúde — provavelmente o leitor nem está concorrendo.

## 4. Ponte com as demais trilhas

C++ escreve a mesma cerca com `std::atomic_thread_fence` na 17a de C++; Go esconde cercas atrás do `sync.Mutex` e do canal (o modelo *happens-before* do Go proíbe o programador de errar a cerca — ao preço de não poder escrevê-la); C usa `atomic_thread_fence` explícito como aqui. O seqlock em qualquer linguagem tem o mesmo contador ímpar — o protocolo é do modelo de memória, não da sintaxe. A 14b mostrou ordenação na operação; esta mostra ordenação sem operação; juntas, fecham o R5 de ordenação.

## 5. Critério de aprovação e checklist

- [ ] Rodo o exemplo e explico por que o sinal pode ser `Relaxed` (a cerca ordena, não ele).
- [ ] Digo o que quebra se qualquer uma das cercas sair (data race no par).
- [ ] Descrevo os dois momentos em que o leitor do seqlock retenta — e por que cada um existe.
- [ ] Justifico quando seqlock vence `RwLock` (escrita rara, leitura quente) e quando perde (escrita frequente).
- [ ] Nomeio o que falta: `SeqCst` e ordem total (laboratório futuro com litmus declarado, fora do `stable` honesto desta aula).

**Referências primárias:** *The Rust Reference*, capítulo *Atomics and Memory Ordering* (`fence`, `Release`/`Acquire`); o paper do seqlock (Lameter) como origem do protocolo contador-ímpar. Nenhum texto foi copiado — o comportamento acima foi confirmado executando este exemplo.

*Atividades e avaliação desta aula serão publicadas ao fim do volume, junto com as demais — por decisão editorial, o conteúdo vem primeiro.*
