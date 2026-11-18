---
layout: default
---

# Aula 11b — Cancelamento, shutdown forçado e GOMAXPROCS (G4)

**Competências SDD:** CON, MEM. **Pré-requisitos:** aulas 10–11 e [11a](11a_modelo_de_memoria.md) (pool limitado, deadlock por projeto, `-race`). Toolchain registrada: Go 1.26.8 (`go vet`, `go run -race`).

**Objetivo observável:** ao terminar, você deverá executar o exemplo, mostrar o pool parando no meio por `context`, o teto de dreno estourando sem pendurar e a mesma soma com `GOMAXPROCS=1` e `=N` — e explicar por que os tempos sob `-race` não entram na tabela.

**Motivação:** a 11a construiu o pool e declarou que o cancelamento cooperativo ficava de fora do exemplo. Esta aula cobra a dívida: `context` que fecha no meio, shutdown que desiste com dignidade (teto em vez de espera infinita) e a comparação de paralelismo que separa ganho de ilusão. Sem isso, o G4 teria pool sem freio — e pool sem freio é vazamento com outro nome.

## 1. Contrato antes do código

O [arquivo-fonte](exemplos/11b_cancelamento.go) contém o programa completo:

```bash
go vet teoria/go/exemplos/11b_cancelamento.go && go run -race teoria/go/exemplos/11b_cancelamento.go
# cancelamento ok: NNNN concluidos de 2000 (parou no meio)
# shutdown ok: teto estourado e reportado, sem espera infinita
# vazamento ok: 1 goroutines antes, 1 depois
# GOMAXPROCS=1 soma=7999998000000 em ~1ms
# GOMAXPROCS=24 soma=7999998000000 em ~0.4ms
# 11b ok: cancelamento obedece, teto forca, GOMAXPROCS comparado sem -race na tabela
```

(O número exato de concluídos varia — o `assert` é sobre a faixa `500 <= n < 2000`, não sobre o valor. Tempo sob `-race` é ordem de grandeza, não tabela: a tabela de speedup se mede sem `-race`, em binário separado.)

## 2. Três mecanismos, três disciplinas

**Cancelamento obedece, não sugere.** O produtor seleciona entre `ctx.Done()` e o envio; o worker, entre `ctx.Done()` e o recebimento. Fechar o contexto no meio (aqui, aos ~500 concluídos) para os dois lados: nada novo entra, quem está dentro termina a iteração e sai. O teste confere a faixa, não o ponto — qualquer ponto entre 500 e 2000 prova obediência; exatamente 500 provaria determinismo falso.

**Shutdown força com teto.** `shutdownComTeto` espera a conclusão até o limite e volta `false` estourado. O exemplo força o estouro de propósito (teto de 1ns): o caminho de falha também é testado, porque shutdown que nunca foi visto falhar é esperança. Em produção o teto é segundos, não nanossegundos — a mecânica é a mesma, a unidade muda.

**GOMAXPROCS compara, `-race` valida.** A mesma soma com 1 e 24: igualdade de resultado é `assert`, tempos são medida sem `-race` (o detector serializa e aloca, e comparar tempo instrumentado com não instrumentado é a fraude que a 28a proíbe). E a contagem de goroutines antes/depois fecha a aula: pool que termina sem vazar é o invariante que o `NumGoroutine` confere.

### Leitura guiada: faixa, estouro, contagem, comparação

`cancelamento ok: NNNN concluidos de 2000` é o assert de faixa em ação: qualquer NNNN entre 500 e 2000 prova que o contexto fechou no meio e os dois lados obedeceram — e a linha não diz o número "certo" porque número certo não existe (o escalonador decide). `shutdown ok` prova o caminho de falha testado: teto de 1ns estourou, foi reportado, ninguém pendurou — shutdown que só foi visto funcionando é esperança, e a aula remove a esperança testando o estouro. `vazamento ok: 1 antes, 1 depois` é o invariante de higiene: goroutine que nasce no pool morre no pool, e a main sozinha antes e depois prova isso sem rastrear nenhuma goroutine pelo nome. As duas linhas de GOMAXPROCS provam a mesma soma em dois mundos (1 e 24) com tempos de binário limpo — igualdade no `assert`, tempos na medida, e a nota do contrato ("tempo sob -race é ordem de grandeza") impede que alguém cite os ~1ms/~0.4s desta página como tabela: são ilustração do formato, não resultado.

## 3. Erros comuns e implicações defensivas

1. **Tempo sob `-race` na tabela:** o detector muda o custo de cada acesso; speedup medido com `-race` não é speedup. Correção com `-race`, tabela sem.
2. **Cancelamento só no produtor:** workers presos em `jobs <- t` sem `select` no `Done` ignoram o cancelamento até a fila esvaziar. Freio tem de estar nos dois lados do canal.
3. **Shutdown sem teto:** `wg.Wait()` puro num pool travado pendura o processo inteiro. Todo `Wait` de desligamento precisa de alternativa com prazo — e a alternativa precisa ser testada (aqui, com 1ns).
4. **Afirmar o ponto de parada:** testar `concluidos == 500` seria flaky por construção — o cancelamento é assíncrono e o número exato depende do escalonador. Faixa se afirma, ponto se observa.
5. **Contar goroutines pelo nome:** rastrear "as do pool" individualmente é frágil e desnecessário — o invariante é aritmético (1 antes, 1 depois), não nominal. Higiene se prova por contagem, não por identidade.
6. **Testar shutdown só no caminho feliz:** teto que nunca estourou no teste é teto presumido — e presumido falha no primeiro pool travado de verdade. O 1ns força o estouro em todo run; caminho de falha sem execução é esperança com nome de teste.

## 4. Ponte com as demais trilhas

Rust faz o mesmo com flag atômica e `scope` na [14b](../rust/14b_ordem_e_backpressure.md) (sem `context`, com `join` estruturado); C++ cancela com atômico + `jthread` na 17a; Python cancela com `Event`/`cancel` no 16b. O padrão "sinal nos dois lados, teto no desligamento, contagem de vivos no fim" é o mesmo nas quatro — muda a sintaxe do sinal. E a disciplina de GOMAXPROCS é a versão Go da comparação 1-núcleo vs máquina cheia que a 28c repete entre processos.

## 5. Critério de aprovação e checklist

- [ ] Rodo o exemplo e explico por que o número de concluídos varia (e por que a faixa é o assert).
- [ ] Mostro o teto estourando e digo onde estaria o `Wait` infinito sem ele.
- [ ] Comparo GOMAXPROCS=1 vs N sem `-race` e digo por que a comparação exige binário limpo.
- [ ] Descrevo o sintoma de cancelar só o produtor (workers drenam a fila inteira).
- [ ] Nomeio o que falta: cancelamento em cascata por `context` com deadline real, backpressure com fila limitada (G4 segue com a 23a).

**Referências primárias:** documentação do pacote `context` (cancelamento cooperativo) e do *Go Memory Model* (o que `close` + `range` e `WaitGroup` sincronizam). Nenhum texto foi copiado — o comportamento acima foi executado com `go run -race`.

*Atividades e avaliação desta aula serão publicadas ao fim do volume, junto com as demais — por decisão editorial, o conteúdo vem primeiro.*
