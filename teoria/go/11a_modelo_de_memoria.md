# Aula 11a - Modelo de memória, corridas e concorrência disciplinada

---

## Objetivo da Aula

Explicar quando uma escrita feita por uma goroutine passa a ser visível para outra — e construir um worker pool limitado que preserva invariantes sob cancelamento e carga. Pré-requisitos: aulas 08b a 11. Não precisa saber `context` em detalhe, rede, `cgo` nem benchmarks formais.

Ao terminar, você deverá distinguir data race de corrida lógica, aplicar happens-before com channels, `Mutex` e `WaitGroup`, evitar deadlock por projeto (não por sorte) e validar tudo com o race detector.

Esta aula é o módulo G4 do mapa: quem domina invariantes sob concorrência aqui carrega o raciocínio para protocolos (G5), sistemas distribuídos (G6) e medição honesta (G7), onde cada invariante violada vira cliente travado ou dado corrompido.

---

## 1. Visibilidade não é imediata: happens-before

Dentro de uma goroutine, o programa executa em ordem. Entre goroutines, nada é garantido sem sincronização: uma escrita pode ficar invisível para outra goroutine para sempre, mesmo sem "erro" visível. A relação *happens-before* diz quais escritas uma leitura é obrigada a enxergar. Em Go, ela nasce de poucos atos:

- `go` (criar goroutine): tudo antes do `go` é visível dentro dela;
- `ch <- v` antes de `<-ch`: o envio sincroniza com o recebimento — o que foi escrito antes do envio é visível depois do recebimento;
- `mu.Lock/unlock`: o que foi escrito antes do `Unlock` é visível depois do próximo `Lock`;
- `wg.Add` antes de `wg.Wait` retornar; `once.Do` antes de qualquer retorno.

Sem um desses atos no caminho, duas goroutines tocando a mesma variável — com ao menos uma escrita — formam um **data race**: comportamento indefinido pelo modelo de memória, mesmo que "funcione no teste".

## 2. Data race não é corrida lógica

- **Data race:** acesso concorrente sem sincronização, um deles escrita. É bug de memória: rode com `-race` e o detector aponta.
- **Corrida lógica:** programa sincronizado, mas o resultado depende da ordem de chegada (dois workers consumindo da mesma fila: quem pega qual job varia). É bug de *projeto* se a ordem importar — e o `-race` fica verde, porque a memória está correta.

Confundir os dois leva ao erro clássico: "passei no `-race`, logo está correto". O detector prova ausência de data race; correção lógica se prova com invariantes e testes.

## 3. Dono do channel e quem fecha

Regra: **quem envia, fecha** — e fecha uma única vez, quando não há mais nada a enviar. Fechar duas vezes entra em pânico; enviar em channel fechado entra em pânico; receber de channel fechado e vazio devolve o zero imediatamente (use a forma `v, ok` para distinguir "vazio por enquanto" de "fechado para sempre"). No pool desta aula, o produtor fecha `jobs` ao terminar; os workers saem pelo `range`, naturalmente.

## 4. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/11a_concorrencia.go) contém exatamente o programa abaixo. Valide com o race detector — ele faz parte do contrato:

```bash
go vet teoria/go/exemplos/11a_concorrencia.go && go run -race teoria/go/exemplos/11a_concorrencia.go
```

```go
package main

import (
	"fmt"
	"sync"
	"sync/atomic"
	"time"
)

// Contador protegido: invariante "acessos sob mutex" verificada pelo -race.
type Contador struct {
	mu sync.Mutex
	n  int64
}

func (c *Contador) Inc() {
	c.mu.Lock()
	c.n++
	c.mu.Unlock()
}

func (c *Contador) Valor() int64 {
	c.mu.Lock()
	defer c.mu.Unlock()
	return c.n
}

// happens-before por channel: escrita antes do envio e visivel apos o recebimento.
func demonstraVisibilidade() {
	pronto := make(chan int)
	go func() {
		soma := 0
		for i := 1; i <= 100; i++ {
			soma += i
		}
		pronto <- soma // envio sincroniza com o recebimento abaixo
	}()
	soma := <-pronto
	if soma != 5050 {
		panic("visibilidade quebrada")
	}
	fmt.Println("visibilidade ok: soma = 5050")
}

// Pool limitado: N workers, fila com teto, dono fecha ao terminar.
func pool(trabalhos []int, workers int) int64 {
	jobs := make(chan int, len(trabalhos))
	var wg sync.WaitGroup
	var total atomic.Int64

	for w := 0; w < workers; w++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			for j := range jobs { // fecha com o close do dono
				total.Add(int64(j * j))
			}
		}()
	}

	for _, t := range trabalhos {
		jobs <- t
	}
	close(jobs) // dono fecha uma vez, ao terminar de enviar
	wg.Wait()
	return total.Load()
}

func main() {
	demonstraVisibilidade()

	// Contador sob concorrencia: 8 goroutines x 10000 incrementos.
	c := &Contador{}
	var wg sync.WaitGroup
	for g := 0; g < 8; g++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			for i := 0; i < 10000; i++ {
				c.Inc()
			}
		}()
	}
	wg.Wait()
	if c.Valor() != 80000 {
		panic("contador perdeu atualizacoes")
	}
	fmt.Println("contador ok: 80000")

	// Pool: 300 jobs, 4 workers.
	trabalhos := make([]int, 300)
	for i := range trabalhos {
		trabalhos[i] = i + 1
	}
	inicio := time.Now()
	total := pool(trabalhos, 4)
	fmt.Printf("pool: total=%d workers=4 duracao=%s\n", total, time.Since(inicio))

	// Invariante verificavel: soma dos quadrados 1..300 = 300*301*601/6.
	const esperado = int64(300 * 301 * 601 / 6)
	if total != esperado {
		panic("pool: invariante violada")
	}
	fmt.Println("pool ok: invariante preservada")
}
```

Saída esperada (duração varia por máquina):

```text
visibilidade ok: soma = 5050
contador ok: 80000
pool: total=9045050 workers=4 duracao=~s
pool ok: invariante preservada
```

O cancelamento cooperativo (`done` + `select`) não está nesta versão: entra no exercício 2, onde fechar `done` no meio do pool prova que todos os workers terminam. Aqui o `range` sobre o channel fechado pelo dono é o mecanismo de parada — simples, e o `go vet` confirma que nada está podre.

### O que a memória faz aqui?

O `Mutex` serializa os acessos ao contador: sem ele, 8 goroutines escrevendo `n++` perdem atualizações e o `-race` denuncia. O channel `jobs` com buffer transfere *happens-before* junto com os dados: o que o produtor escreveu antes de cada envio é visível ao worker que recebe. Nenhum `Sleep` foi usado para "dar tempo" — sincronização por tempo é corrida lógica disfarçada de correção.

## 5. Deadlock se projeta para fora

Deadlock clássico: todas as goroutines dormindo em `recv` sem ninguém para enviar — o runtime detecta e aborta com `fatal error: all goroutines are asleep`. As defesas são de desenho: dono fecha o channel, `select` com `done`/`timeout`, fila com teto (`len(trabalhos)` no buffer evita produtor travado). Se seu programa precisa de `time.Sleep` para "funcionar", ele tem uma corrida lógica com temporizador.

## 6. Erros comuns e implicações defensivas

1. **Achar que `-race` limpo é prova total:** prova ausência de data race. Lógica se prova com invariante e teste.
2. **Fechar channel duas vezes ou enviar após fechar:** pânico. Um dono, um `close`, no fim do envio.
3. **Ignorar o segundo retorno do receive:** sem `ok`, "fechado e vazio" parece "dado zero válido".
4. **Fila sem teto:** `jobs := make(chan int)` sem buffer trava o produtor até haver worker livre; com carga, vira deadlock sob pressão. Dimensione o buffer ou use `select` com desistência.
5. **Copiar `Mutex` por valor:** `go vet` acusa (`copylocks`). Passe por ponteiro.
6. **Contador sem sincronização "porque é só um int":** é o data race mais comum do Go real. Atômico ou mutex, sempre.

## 7. Ponte com Python e JavaScript

Python (`threading`) sofre do GIL para CPU-bound e exige `Lock` para o mesmo contador; `multiprocessing` isola por processo e paga IPC. JavaScript (Node) serializa num event loop — sem data race entre callbacks, mas com as mesmas corridas lógicas em operações assíncronas concorrentes. O modelo muda, a disciplina não: dono do canal/fila, fechamento único, cancelamento cooperativo.

## 8. Exercícios obrigatórios

1. Reescreva as quatro fontes de happens-before da seção 1 sem consultar o texto, com um exemplo mínimo de cada.
2. Ligue o cancelamento: feche `done` no meio do pool e prove que todos os workers terminam. Meça trabalhos perdidos vs. concluídos.
3. Remova o `Mutex` do contador, rode com `-race` e transcreva o WARNING. Depois restaure e confirme o silêncio.
4. Troque o buffer da fila para 0 e explique quando o produtor trava. Documente o teto adequado para 300 jobs e 4 workers.

Use [pistas graduais](exercicios/11a_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/11a_solucoes.md) depois de registrar sua própria explicação.

## 9. Critério de aprovação e checklist

Você concluiu a aula se consegue, sem olhar a solução:

- definir happens-before e citar os quatro atos que o criam;
- distinguir data race de corrida lógica e dizer o que cada um exige como prova;
- enunciar quem fecha o channel, quando e por quê;
- rodar o exemplo com `-race` limpo e explicar cada invariante checada;
- desenhar um pool com fila limitada e cancelamento sem `Sleep` de sincronização;
- apontar o que o `-race` garante e o que ele não garante.

**Próxima aula:** generics. O raciocínio desta aula sustenta coleções concorrentes: o que a estrutura garante sob acesso simultâneo.
