// Aula 11b Go — Cancelamento, shutdown forcado e GOMAXPROCS.
//
// A 11a deixou o cancelamento cooperativo de fora do exemplo ("entra no
// exercicio 2"). Esta aula o traz para o conteudo: pool com `context`,
// shutdown com dreno limitado (forca a saida quando o dreno estoura o
// teto) e a comparacao 1 x N `GOMAXPROCS` que separa ganho real de
// ilusao do `-race` (tempo sob `-race` nao entra na tabela).
//
// Validar: go vet teoria/go/exemplos/11b_cancelamento.go && go run -race teoria/go/exemplos/11b_cancelamento.go
package main

import (
	"context"
	"fmt"
	"runtime"
	"sync"
	"sync/atomic"
	"time"
)

// Pool que obedece ao contexto: sem trabalho novo apos o cancelamento,
// e cada worker registra a saida para o teste de vazamento conferir.
func poolCancelavel(ctx context.Context, trabalhos []int, workers int, concluidos *atomic.Int64) {
	jobs := make(chan int)
	var wg sync.WaitGroup
	for w := 0; w < workers; w++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			for {
				select {
				case <-ctx.Done():
					return
				case j, ok := <-jobs:
					if !ok {
						return
					}
					_ = j * j
					concluidos.Add(1)
				}
			}
		}()
	}
	for _, t := range trabalhos {
		select {
		case <-ctx.Done():
			break
		case jobs <- t:
		}
	}
	close(jobs)
	wg.Wait()
}

// Shutdown com dreno limitado: tenta terminar em ate `teto`; estourado,
// desiste e relata quantos ficaram (forcar nao e esperar para sempre).
func shutdownComTeto(concluidos *atomic.Int64, total int, teto time.Duration) (ok bool) {
	fim := time.After(teto)
	for {
		if concluidos.Load() >= int64(total) {
			return true
		}
		select {
		case <-fim:
			return false
		case <-time.After(time.Millisecond):
		}
	}
}

func medeGomaxprocs() {
	n := 4000000
	soma := func() int64 {
		var total atomic.Int64
		var wg sync.WaitGroup
		for w := 0; w < 8; w++ {
			wg.Add(1)
			go func(fatia int) {
				defer wg.Done()
				var local int64
				for i := fatia; i < n; i += 8 {
					local += int64(i)
				}
				total.Add(local)
			}(w)
		}
		wg.Wait()
		return total.Load()
	}
	var esperada int64
	for i := 0; i < n; i++ {
		esperada += int64(i)
	}
	for _, g := range []int{1, runtime.NumCPU()} {
		runtime.GOMAXPROCS(g)
		t0 := time.Now()
		obtida := soma()
		dt := time.Since(t0)
		if obtida != esperada {
			panic("soma divergiu: bug, nao GOMAXPROCS")
		}
		fmt.Printf("GOMAXPROCS=%d soma=%d em %v\n", g, obtida, dt)
	}
}

func main() {
	antes := runtime.NumGoroutine()
	var concluidos atomic.Int64
	ctx, cancela := context.WithCancel(context.Background())
	trabalhos := make([]int, 2000)
	for i := range trabalhos {
		trabalhos[i] = i
	}
	// Cancela no meio: o pool para de aceitar e os workers saem.
	go func() {
		for concluidos.Load() < 500 {
			time.Sleep(time.Millisecond)
		}
		cancela()
	}()
	poolCancelavel(ctx, trabalhos, 8, &concluidos)
	parados := concluidos.Load()
	fmt.Printf("cancelamento ok: %d concluidos de 2000 (parou no meio)\n", parados)
	if parados < 500 || parados >= 2000 {
		panic("cancelamento nao obedeceu")
	}

	// Shutdown forcado: teto curto demais para o restante estoura de proposito.
	var concluidos2 atomic.Int64
	ctx2, cancela2 := context.WithCancel(context.Background())
	defer cancela2()
	go poolCancelavel(ctx2, trabalhos, 8, &concluidos2)
	// Da tempo de engrenar e cancela: o dreno de 1ns nao termina.
	for concluidos2.Load() == 0 {
		time.Sleep(time.Millisecond)
	}
	cancela2()
	if shutdownComTeto(&concluidos2, len(trabalhos), time.Nanosecond) {
		panic("teto de 1ns nao deveria bastar")
	}
	fmt.Println("shutdown ok: teto estourado e reportado, sem espera infinita")

	// Sem vazamento: da tempo das goroutines sairem e confere.
	time.Sleep(200 * time.Millisecond)
	depois := runtime.NumGoroutine()
	if depois > antes+2 {
		panic(fmt.Sprintf("vazamento: %d antes, %d depois", antes, depois))
	}
	fmt.Printf("vazamento ok: %d goroutines antes, %d depois\n", antes, depois)

	medeGomaxprocs()
	fmt.Println("11b ok: cancelamento obedece, teto forca, GOMAXPROCS comparado sem -race na tabela")
}
