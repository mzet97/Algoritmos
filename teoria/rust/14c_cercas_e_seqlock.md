---
layout: default
---

# Aula 14c — Cercas e seqlock: ordenação sem operação e leitura otimista (R5)

## Nesta aula

Você vai aprender a explicar cercas de memória e leitura otimista (seqlock), distinguir ordenação sem operação atômica do custo de um lock, e apontar quando a técnica é aplicável ou proibida.

Você vai ao terminar, você deverá executar o exemplo, mostrar 50 mil publicações conferidas par a par via `fence` (sem nenhum `Release`/`Acquire` nas operações) e explicar por que o leitor do seqlock retenta em vez de bloquear — e o que o contador ímpar proíbe.

a 14b ensinou ordenação carregada na operação: `store(Release)` publica, `load(Acquire)` consome. Mas ordenar e mover dado são coisas distintas, e o hardware distingue as duas. A `fence` ordena sem operação — publica um par de `u64` comum com `Relaxed` nas pontas e cerca no meio. E o seqlock mostra para que isso serve: leitor que nunca bloqueia escritor, ao preço de retentar quando pega escrita no meio. É o padrão de "ler muito, escrever pouco" que aparece em relógio, config e versão de estrutura.

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

### O litmus de cada cerca removida (o que cada uma proíbe)

Faça o exercício mental de remover uma peça por vez e dizer qual execução proibida volta a ser possível: sem o `fence(Release)` do produtor, as duas escritas no par podem ficar visíveis depois do sinal — o consumidor vê `pronto`, lê o par e pega `[i, lixo]`; sem o `fence(Acquire)` do consumidor, a leitura do par pode subir para antes da leitura do sinal — o consumidor lê dado velho com sinal novo. Sem nenhuma cerca, as duas reordenações compõem: qualquer combinação de velho/novo em cada posição. E trocar os `Relaxed` por `Release`/`Acquire` nas operações continuaria correto — mas pagaria ordenação por endereço onde bastava ordenação por cerca, e esconderia a lição: o sinal não carrega dado, então não precisa carregar ordenação. A forma mínima que continua correta é a forma que ensina: `Relaxed` nas pontas, cerca no meio, e cada peça com um litmus que justifica sua existência.

## 2. O seqlock: leitor otimista, escritor sem espera

O escritor incrementa o contador para ímpar (`Release`), escreve, incrementa para par (`Release`). O leitor lê o contador (`Acquire`); se ímpar, retenta; lê o dado; relê o contador — se mudou, retenta. O contador ímpar proíbe leitura: não existe "ler durante escrita e torcer". O par diferente proíbe uso: não existe "li metade velha, metade nova".

O preço aparece no número de retentativas (centenas de milhares para 50 mil leituras nesta máquina): o leitor gira enquanto o escritor escreve. Seqlock serve quando escrita é rara e leitura é quente — config lida por milhares de threads, atualizada uma vez por minuto. Com escrita frequente, o leitor morre de fome girando: aí é `RwLock` (aula 13) ou versão com espera, não otimismo.

### Leitura guiada: três linhas, dois protocolos

A primeira linha (`cercas ok: 50000 publicacoes conferidas par a par`) prova publicação ordenada sem operação ordenada: cada um dos 50 mil pares `[i, i*7]` foi lido íntegro do outro lado das cercas — um único `[i, lixo]` teria quebrado o `assert` e a linha não sairia. A segunda (`seqlock ok: 50000 valores consistentes, N retentativas`) prova o leitor otimista: 50 mil valores íntegros, e o `N` conta quantas vezes o protocolo descartou leitura a meio — retentativa aqui é trabalho do protocolo funcionando, não contenção falhando; `N == 0` com escrita concorrente real seria o suspeito (leitor que nunca retenta provavelmente nunca concorreu). A terceira fecha o par: cerca publica, contador ordena, leitor nunca trava escritor. E o parêntese do contrato ("o número de retentativas varia") separa veredito de medida: o `assert` cobra consistência exata, o `N` informa contenção — teste que exige `N` exato quebraria na próxima máquina sem que nada estivesse errado.

## 3. Erros comuns e implicações defensivas

1. **Cerca só de um lado:** `Release` sem `Acquire` (ou vice-versa) não sincroniza nada — é como lacrar a carta e o destinatário não olhar o lacre. Paridade de cercas é contrato, não estilo.
2. **Ler com contador ímpar "só para ver":** o valor lido durante escrita é Frankenstein (metade velha). O ramo `if antes % 2 == 1` não é otimização, é correção.
3. **Esquecer a segunda leitura do contador:** ler, checar ímpar, usar — sem reler depois — aceita escrita que começou depois da primeira leitura. As duas leituras são o par; uma só é metade do protocolo.
4. **Seqlock com escrita frequente:** otimismo sem espera vira espera sem progresso para o leitor. Medir retentativas (o exemplo imprime) é o termômetro: retentativa perto de zero com escrita constante é suspeita, não saúde — provavelmente o leitor nem está concorrendo.
5. **`SeqCst` "por segurança":** trocar tudo por `SeqCst` funciona e ensina nada — a ordem total esconde qual par realmente precisa se ver. Comece pelo mais fraco que o protocolo exige (`Relaxed` + cercas aqui) e suba só quando o litmus provar necessidade; força máxima por padrão é confissão de que não se sabe o que cada peça ordena.
6. **Exigir `N` de retentativas exato no teste:** contenção depende de escalonador, carga e máquina — `assert` em `N` é teste da máquina, não do protocolo. O exemplo acerta ao cobrar consistência exata e só informar `N`: veredito no que é determinístico, medida no que não é.

## 4. Ponte com as demais trilhas

C++ escreve a mesma cerca com `std::atomic_thread_fence` na 17a de C++; Go esconde cercas atrás do `sync.Mutex` e do canal (o modelo *happens-before* do Go proíbe o programador de errar a cerca — ao preço de não poder escrevê-la); C usa `atomic_thread_fence` explícito como aqui. O seqlock em qualquer linguagem tem o mesmo contador ímpar — o protocolo é do modelo de memória, não da sintaxe. A 14b mostrou ordenação na operação; esta mostra ordenação sem operação; juntas, fecham o R5 de ordenação.

## 5. Critério de aprovação e checklist

- [ ] Rodo o exemplo e explico por que o sinal pode ser `Relaxed` (a cerca ordena, não ele).
- [ ] Digo o que quebra se qualquer uma das cercas sair (data race no par).
- [ ] Descrevo os dois momentos em que o leitor do seqlock retenta — e por que cada um existe.
- [ ] Justifico quando seqlock vence `RwLock` (escrita rara, leitura quente) e quando perde (escrita frequente).
- [ ] Executo o litmus de cada cerca removida e digo qual execução proibida volta.
- [ ] Explico por que o teste cobra consistência exata mas só informa `N`.
- [ ] Nomeio o que falta: `SeqCst` e ordem total (laboratório futuro com litmus declarado, fora do `stable` honesto desta aula).

**Referências primárias:** *The Rust Reference*, capítulo *Atomics and Memory Ordering* (`fence`, `Release`/`Acquire`); o paper do seqlock (Lameter) como origem do protocolo contador-ímpar. Nenhum texto foi copiado — o comportamento acima foi confirmado executando este exemplo.

## 6. Exercícios obrigatórios

1. (Previsão) Apague o `fence(Release)` da publicação (mantenha o `Acquire` no consumo): preveja — o `assert` de par quebra no x86-64? Rode e explique por que "passou aqui" não prova nada (TSO do x86 salva; ARM não salvaria) — e por que o teste verde numa máquina só é metade da evidência.
2. (Diagnóstico) Apague a checagem de ímpar no `le` (leia mesmo com `seq` ímpar): preveja — continua passando? Rode 3×, compare as retentativas (~1M com a checagem, ~4k sem) e explique quem cobre o que (`antes % 2` retenta toda concorrência; `antes != depois` pega a rasgada) — e qual buraco teórico resta nas duas versões (ciclo completo par→par entre as duas leituras).
3. (Projeto) Separe as causas de retry em dois contadores (`impar` vs `mudou`): rode e mostre os dois números. Explique a proporção (~360k ímpares, ~50 mudanças): a checagem de ímpar retenta escrita em curso mesmo quando o dado sairia intacto — otimismo tem preço, e o preço é mensurável.
4. (Previsão) Troque o sinal `PRONTO` (`Relaxed` + cercas) por `Release`/`Acquire` grudados no `store`/`load`: preveja — continua passando? Rode e explique quando a cerca separada vale a pena (ordenar dado não-atômico sem operação que carregue dado) e quando o ordenamento grudado basta.

Use [pistas graduais](exercicios/14c_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/14c_solucoes.md) depois de registrar sua própria explicação.

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
