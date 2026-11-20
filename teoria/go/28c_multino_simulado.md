---
layout: default
---

# Aula 28c — Multinó simulado: a tríade em dois processos (G7)

## Nesta aula

Você vai aprender a simular multinó em dois processos aplicando a tríade (framing, quorum, relógios), confrontar os resultados, e declarar limites da simulação.

Você vai ao terminar, você deverá executar o exemplo, mostrar o checksum combinado igual à referência local, apontar no código onde o deadline limita cada fase do protocolo — e explicar por que a razão medida aqui não prova nada sobre multinó real.

a 28b contou um nó NUMA e declarou o multinó como simulado; o G7 pede o multinó "simulado declarado" virar procedimento. Esta aula atravessa a fronteira: a mesma tríade, metade calculada em cada um de dois processos conversando por TCP em `127.0.0.1`, checksum confrontado com a referência de um processo só. O que viaja é metadado (fatia + checksum + tempo); o dado é derivado da mesma semente nos dois lados — mover 32 MiB pelo loopback para provar soma seria medir o loopback, não o multinó.

## 1. Contrato antes do código

O [arquivo-fonte](exemplos/28c_multino.go) contém coordenador e trabalhador (o mesmo binário, `argv` decide; porta efêmera):

```bash
go vet teoria/go/exemplos/28c_multino.go && go run -race teoria/go/exemplos/28c_multino.go
# referencia local: ~0.5 GB/s em ~90ms (sob -race; ordem de grandeza, nao tabela)
# duas metades remotas: pior em ~50ms (loopback, 1 no)
# razao remoto/local: ~0.5x (medida, nao veredito)
# 28c ok: checksum combinado igual a referencia, protocolo com deadline, multino simulado declarado
```

(Os tempos variam com máquina e carga — e sob `-race` são piores por construção. O `assert` é sobre o checksum; a tabela de speedup se mede sem `-race`, como a 28a manda. A razão < 1 aqui reflete duas metades em paralelo contra referência serial: paralelismo real, transporte simulado.)

## 2. Protocolo mínimo que já é distribuído de verdade

**Descoberta sem config:** o coordenador escuta em porta efêmera e passa o endereço por `argv` ao `exec` do trabalhador (`os.Executable()` — o binário se replica, sem segundo artefato). Sem config, sem DNS, sem service discovery fingido: em dois nós reais, esse `argv` seria o inventário — e a aula diz isso em vez de inventar um.

**Deadline em cada fase:** `SetDeadline` cobre accept, leitura da fatia e resposta. Trabalhador que trava não pendura o coordenador: estoura o deadline e o erro acusa qual fase parou. É a disciplina da 25 (timeouts) aplicada ao HPC: sem deadline, "multinó" vira "esperança com dois computadores".

**Dado derivado, não movido:** as duas metades geram `b` e `c` da mesma fórmula; só `ini/fim` viajam na ida e `checksum + nanos` na volta. Em multinó real com dado particionado de verdade, o custo de mover a partição entraria na conta da viagem (28a) — aqui o protocolo isola o custo de coordenação do custo de transporte, e declara a isolação.

### Leitura guiada: por que a razão < 1 é o resultado certo

`referencia local: ~0.5 GB/s` é a âncora serial sob `-race` — ordem de grandeza publicada, não tabela. `duas metades remotas: pior em ~50ms` parece contradição (duas metades em paralelo piores que a referência?) até lembrar que a referência é serial de 64 MiB e cada metade processa 32 MiB em paralelo: o wall-clock cai, mas a soma dos tempos sobe — paralelismo real, transporte simulado. `razao remoto/local: ~0.5x` mede coordenação + paralelismo sobre loopback, não multinó: publicá-la como "multinó perde" seria a fraude que o contrato proíbe ("medida, não veredito"). O veredito (`checksum combinado igual a referencia`) é o que atravessa processo: a mesma tríade, metade calculada do outro lado de um socket, checksum idêntico — prova que o protocolo transporta trabalho corretamente, que é tudo que um nó só pode provar. Quem cita 0.5x sem "loopback, 1 nó, referência serial" está citando outro experimento.

## 3. Erros comuns e implicações defensivas

1. **Comparar tempo com `-race`:** o detector serializa e aloca; referência local sob `-race` contra metades sob `-race` ainda compara certo (ambos instrumentados), mas tabela contra binário limpo é fraude. Correção com `-race`, tabela sem.
2. **Mover o dado para "ser realista":** enviar 32 MiB pelo loopback mede o loopback com precisão e o multinó com zero. Realismo de transporte se mede com `numactl`/RDMA de verdade (futuro), não com socket local gordo.
3. **Sem deadline "porque é localhost":** teste que pendura no localhost pendura pior na rede. Deadline curto e erro nomeado por fase desde o primeiro dia.
4. **Chamar de "medição multinó":** um nó, um loopback, duas metades em paralelo. O que se mediu: coordenação + paralelismo. O que falta para multinó: latência real, partição real, falha parcial real (a 23b mostra como injetar as duas últimas).
5. **Somar tempos em vez de medir wall-clock:** duas metades de ~50ms cada não são "100ms de multinó" — são 50ms de parede com 2× de recurso. Speedup se mede em relógio de parede contra a referência serial; soma de tempos parciais é contabilidade, não desempenho.
6. **Derivar o dado dos dois lados sem confrontar a semente:** se cada metade gera `b`/`c` de fórmula "igual" mas com constante divergente, os checksums diferem e o erro parece de protocolo. O checksum combinado só prova o transporte se a derivação é idêntica — e idêntica se prova confrontando a referência local primeiro (a ordem das linhas da saída não é acaso).

## 4. Ponte com as demais trilhas

Rust mede a mesma tríade com `taskset` na [23e](../rust/23e_afinidade_e_numa.md) (um processo, afinidade externa); C conta o nó na 30c; C++ modela transferência sem placa na 30b. O par "checksum afirma, tempo publica, máquina acompanha o número" é o mesmo nas quatro — aqui com um ingrediente a mais: o checksum atravessa processo, então prova protocolo além de conta. Em Python, o análogo honesto é `multiprocessing` com a mesma derivação de dado — e a mesma declaração de nó único.

## 5. Ao final, você consegue

- [ ] Rodo o exemplo e mostro o checksum combinado igual à referência (e digo por que é igualdade, não tolerância).
- [ ] Aponto os três deadlines e o erro que cada um produz.
- [ ] Explico por que o dado é derivado (e o que mudaria se fosse movido).
- [ ] Digo por que a razão medida não prova multinó (loopback, 1 nó, referência serial).
- [ ] Nomeio o que falta: 2 nós reais, partição com transporte, falha parcial, RDMA (G7 segue aberto).

**Referências primárias:** documentação do pacote `net` (deadlines como contrato, não cortesia) e do `os/exec` (o binário como seu próprio trabalhador). Nenhum texto foi copiado — os números acima foram medidos nesta máquina.

## 6. Exercícios obrigatórios

1. (Previsão) Troque só no trabalhador `gera(c, 3.0)` por `gera(c, 3.5)` (coordenador intacto): preveja se o checksum combinado converge. Rode e explique com os dois números — o que o checksum prova, a conta ou o transporte.
2. (Diagnóstico) Troque no `OK` o `%.17g` por `%.6g`: preveja o veredito. Rode e explique por que o protocolo exige round-trip de `float64` — quantos dígitos `%.6g` entrega e por que a soma acusa.
3. (Projeto) Faça o trabalhador responder `ERRO induzido` em vez de `OK`: preveja a mensagem do coordenador e qual fase do protocolo acusa (accept, envio da fatia ou leitura da resposta). Rode e explique por que o deadline de 60 s não precisou estourar.
4. (Previsão) Rode sem `-race` (`go run teoria/go/exemplos/28c_multino.go`): preveja se o checksum muda e se a razão muda. Rode e explique — com os dois pares de números — por que correção se verifica com `-race` e tabela sem.

Use [pistas graduais](exercicios/28c_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/28c_solucoes.md) depois de registrar sua própria explicação.

---

### Como esta aula será avaliada

| Critério | Peso |
| -------- | ---: |
| Conceitos e vocabulário (nível 1) | 20% |
| Previsão de comportamento e diagnóstico de erros (nível 2) | 40% |
| Projeto e justificativa de decisões (nível 3) | 30% |
| Registro de comandos, saídas e interpretação | 10% |

**Aprovação:** nota mínima 7,0; pelo menos 60% dos pontos devem vir de previsão e diagnóstico (níveis 2 e 3), não apenas de código que executa.

**Para se aprofundar:**

1. *Designing Data-Intensive Applications*, Martin Kleppmann (O'Reilly, ISBN 978-1449373320); 2. Documentação oficial em [go.dev/doc](https://go.dev/doc/); 3. *Site Reliability Engineering* (Google, O'Reilly, ISBN 978-1491929124).
