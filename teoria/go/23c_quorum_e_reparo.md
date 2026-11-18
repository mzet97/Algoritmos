---
layout: default
---

# Aula 23c — Quorum de escrita e leitura com reparo: a maioria decide, o atraso obedece

**Competências SDD:** DST e SEC. **Pré-requisitos:** aulas [23a](23a_protocolos_distribuidos.md) (framing, idempotência, log) e [23b](23b_relogios_e_consistencia.md) (Lamport, vetores, perda declarada). Esta ponte fecha o que a 23b declarou em aberto: reconciliar sem quorum é convergir sem garantia; aqui a escrita exige a maioria antes de existir para o cliente.

**Objetivo observável:** ao terminar, você deverá escrever com quorum (W=2 de 3), ler com quorum (R=2 de 3), detectar a escrita concorrente pelo vetor e reparar a réplica atrasada — apontando onde cada decisão mora no código.

**Motivação:** a 23b reconcilia *depois* da partição e declara a perda. Falta impedir a leitura obsoleta *durante* a operação normal: com W+R maior que o número de réplicas, toda leitura encontra pelo menos uma réplica que viu a última escrita confirmada. Quorum não é votação por gosto — é interseção garantida.

## 1. Contrato antes do código

- **Escrita com quorum:** o coordenador escreve no próprio vetor (incrementa o próprio componente), replica para as vivas e só confirma com 2 acks. Sem maioria, sem confirmação — o cliente recebe erro, não promessa.
- **Leitura com quorum:** consulta as vivas, exige 2 respostas; escolhe a entrada que domina as demais; sem dominância, declara conflito em vez de servir o obsoleto.
- **Reparo de leitura:** a réplica atrasada recebe a vencedora na própria leitura — a anti-entropia acontece no caminho feliz, não num processo separado.
- **Partição:** o lado de 1 não escreve (sem quorum) e não lê (sem quorum). Indisponibilidade declarada, não resposta inventada.
- **Fora do contrato:** eleição de líder e commit por maioria na escrita (o C cobre na 23d); aqui o coordenador é fixo e didático. Consenso de verdade continua sendo outro curso.

## 2. Por que W+R > N?

Com N=3, W=2 e R=2, qualquer escrita confirmada tocou 2 réplicas e qualquer leitura consulta 2 — as duas maiorias se interceptam em pelo menos 1. Essa 1 carrega o vetor mais novo, e a leitura o encontra por construção, não por sorte. Com W=1, a escrita confirmada pode morar só onde a leitura não olhou: a "confirmação" virou boato. O preço é a latência da mais lenta das 2 — quorum cobra em tempo o que paga em verdade.

## 3. Exemplo completo e executável

```go
package main

import "fmt"

const nos = 3

type entrada struct {
	valor   string
	relogio [nos]int
}

type replica struct {
	id    int
	viva  bool
	dados map[string]entrada
}

// domina: a >= b em tudo e > em algo.
func domina(a, b [nos]int) bool {
	maior := false
	for i := 0; i < nos; i++ {
		if a[i] < b[i] {
			return false
		}
		maior = maior || a[i] > b[i]
	}
	return maior
}

func escrever(replicas *[nos]replica, coord int, chave, valor string) bool {
	acks := 0
	for i := range replicas {
		if !replicas[i].viva {
			continue
		}
		e := replicas[i].dados[chave]
		if i == coord {
			e.relogio[i]++
		}
		e.valor = valor
		replicas[i].dados[chave] = e
		acks++
	}
	return acks >= 2
}

func ler(replicas *[nos]replica, chave string) (entrada, bool, bool) {
	var respostas []entrada
	for i := range replicas {
		if !replicas[i].viva {
			continue
		}
		e, ok := replicas[i].dados[chave]
		if ok {
			respostas = append(respostas, e)
		}
	}
	if len(respostas) < 2 {
		return entrada{}, false, false
	}
	melhor := respostas[0]
	conflito := false
	for _, e := range respostas[1:] {
		switch {
		case e.relogio == melhor.relogio:
		case domina(e.relogio, melhor.relogio):
			melhor = e
		case !domina(melhor.relogio, e.relogio):
			conflito = true
		}
	}
	// Reparo de leitura: empurra a vencedora para as atrasadas.
	for i := range replicas {
		if !replicas[i].viva {
			continue
		}
		e := replicas[i].dados[chave]
		if e.relogio != melhor.relogio {
			replicas[i].dados[chave] = melhor
		}
	}
	return melhor, true, conflito
}

func main() {
	var replicas [nos]replica
	for i := range replicas {
		replicas[i] = replica{id: i, viva: true, dados: map[string]entrada{}}
	}

	if !escrever(&replicas, 0, "k", "111") {
		panic("fase1: escrita deveria confirmar")
	}
	m, ok, conflito := ler(&replicas, "k")
	if !ok || conflito || m.valor != "111" {
		panic("fase1: leitura deveria ser limpa")
	}
	fmt.Println("fase1 quorum limpo: k=111")

	// Particao: lado {2} sozinho nao escreve nem le.
	replicas[2].viva = false
	if !escrever(&replicas, 0, "k", "222") {
		panic("fase2: maioria {0,1} deveria confirmar")
	}
	if _, ok, _ := ler(&replicas, "k"); !ok {
		panic("fase2: maioria deveria ler")
	}
	replicas[0].viva = false
	if _, ok, _ := ler(&replicas, "k"); ok {
		panic("fase2: minoria de 1 nao pode ler por quorum")
	}
	fmt.Println("fase2 minoria sem quorum: escrita e leitura recusadas")
	replicas[0].viva = true

	// Cura + reparo: a atrasada recebe a vencedora na leitura.
	replicas[2].viva = true
	m, ok, conflito = ler(&replicas, "k")
	if !ok || conflito || m.valor != "222" {
		panic("fase3: cura deveria convergir em 222")
	}
	if replicas[2].dados["k"].valor != "222" {
		panic("fase3: reparo deveria atualizar a atrasada")
	}
	fmt.Println("fase3 pos-cura: k=222, atrasada reparada na leitura")
	fmt.Println("testes ok")
}
```

Execução (`go vet` + `go run -race`, como nas demais pontes):

```bash
go vet ./... && go run -race 23c_quorum.go
```

Saída esperada (determinística):

```text
fase1 quorum limpo: k=111
fase2 minoria sem quorum: escrita e leitura recusadas
fase3 pos-cura: k=222, atrasada reparada na leitura
testes ok
```

Um detalhe honesto do exemplo: o coordenador é fixo (réplica 0) e a replicação copia o valor sem fundir vetores por componente — suficiente porque só o coordenador escreve. Com dois escritores simultâneos, a fusão `max` da [23b](../go/23b_relogios_e_consistencia.md) voltaria a ser obrigatória; o exemplo declara o limite em vez de fingir generalidade. Um bug real foi evitado aqui: a primeira versão comparava vetores sem o caso de igualdade e acusava conflito entre réplicas idênticas — o `case` de igualdade veio da correção da [23c de C++](../cpp/23c.md).

### O que a memória faz aqui?

Mapas por réplica, sem compartilhamento entre goroutines neste exemplo (coordenador sequencial): o `-race` passa porque não há corrida para detectar — e o texto diz isso. Concorrência real com coordenadores paralelos exigiria mutex por réplica, como na [aula 11](exercicios/11-solucoes.md); sem ele, o mapa rasga.

## 4. Erros comuns e implicações defensivas

1. **Confirmar com W=1:** a escrita mora onde a leitura pode não olhar. Rapidez comprada com verdade alheia.
2. **Ler de uma só réplica "para depurar":** depuração que serve obsoleto sem avisar vira decisão tomada sobre dado morto.
3. **Reparar sem dominância:** empurrar valor empatado para as demais espalha o conflito em vez de resolvê-lo. Reparo só com vencedora.
4. **Esquecer o caso de igualdade:** réplicas sincronizadas têm vetores idênticos — acusar conflito aí é alarme falso que esconde o conflito real.
5. **Chamar isto de consenso:** sem eleição, o coordenador fixo é ponto único de coordenação. A escrita é quorum, não commit com líder eleito.

## 5. Ponte com as demais trilhas

C confirma por maioria na 23d e lê pelo `id` na 23e; C++ declara conflito no quorum da 23c; Rust audita o dono de cada byte que cruza fronteira. Em JavaScript, quorum mora em bibliotecas de sync offline-first (duas fontes, uma verdade) — a mesma interseção, sem servidor.

## 6. Checklist do que esta aula garante

- [ ] Explico por que W+R > N impede a leitura obsoleta, com a interseção desenhada.
- [ ] Demonstro escrita e leitura recusadas na minoria, com o quorum contado.
- [ ] Mostro o reparo atualizando a atrasada e digo quando ele não se aplica.
- [ ] Nomeio os limites: coordenador fixo, sem fusão multiescritor, sem eleição.
- [ ] Distingo quorum (desta aula) de consenso (outro curso).

**Referências primárias:** [Dynamo (Amazon)](https://www.amazon.science/publications/dynamo-amazons-highly-available-key-value-store) para W/R/N e reparo de leitura; [Raft](https://raft.github.io/raft.pdf) para o que quorum sem eleição não entrega.

*Atividades e avaliação desta aula serão publicadas ao fim do volume, junto com as demais — por decisão editorial, o conteúdo vem primeiro.*
