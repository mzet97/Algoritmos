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
