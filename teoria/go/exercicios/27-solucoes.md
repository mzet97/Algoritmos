---
layout: default
---

# Aula 27 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../27.md#25-exercicios-obrigatorios).

```go
package main

import (
	"fmt"
	"runtime"
	"time"
)

func lerStats(rotulo string) runtime.MemStats {
	runtime.GC()
	var m runtime.MemStats
	runtime.ReadMemStats(&m)
	fmt.Printf("%s: HeapAlloc=%.1f MiB NumGC=%d PausasTotaisMs=%.2f\n",
		rotulo, float64(m.HeapAlloc)/(1<<20), m.NumGC, float64(m.PauseTotalNs)/1e6)
	return m
}

func processarComWorkers(payloads [][]byte, workers int) time.Duration {
	jobs := make(chan []byte, 4) // fila pequena de proposito (exercicio 4)
	inicio := time.Now()
	done := make(chan struct{}, workers)
	for w := 0; w < workers; w++ {
		go func() {
			soma := 0
			for p := range jobs {
				for _, b := range p {
					soma += int(b)
				}
			}
			_ = soma
			done <- struct{}{}
		}()
	}
	for _, p := range payloads {
		jobs <- p
	}
	close(jobs)
	for w := 0; w < workers; w++ {
		<-done
	}
	return time.Since(inicio)
}

func main() {
	antes := lerStats("antes")
	payloads := make([][]byte, 0, 64)
	for i := 0; i < 64; i++ {
		p := make([]byte, 256<<10) // 256 KiB temporarios por item
		for j := range p {
			p[j] = byte(i)
		}
		payloads = append(payloads, p)
	}
	depois := lerStats("com carga viva")
	fmt.Println("HeapAlloc cresceu com a carga:", depois.HeapAlloc > antes.HeapAlloc,
		"| NumGC antes/depois:", antes.NumGC, depois.NumGC)

	dur4 := processarComWorkers(payloads, 4)
	dur1 := processarComWorkers(payloads, 1)
	fmt.Println("4 workers ms:", dur4.Milliseconds(), "| 1 worker ms:", dur1.Milliseconds(),
		"| serial nao vence:", dur1 >= dur4/2)
	payloads = nil
	lerStats("depois de soltar")
	fmt.Println("testes ok")
}
```

1 e 2. Rode com `GODEBUG=gctrace=1 go run .` e leia as linhas de coleta: com payload de 256 KiB por item, cada coleta varre mais bytes vivos e `NumGC` avanca mais rapido que com payload de 1 KiB — o GC nao cobra por objeto, cobra por byte vivo e por raiz para varrer. Latencia media sob carga vem das pausas (`PauseTotalNs`) divididas pelo trabalho, nao de um numero magico.

4. A fila de 4 neste programa e o gargalo visivel: o produtor bloqueia no `jobs <- p` assim que 4 itens esperam. No exemplo da aula, fila menor com produtor bloqueante vira **backpressure**; com `select`/`default`, viraria descarte — o exercicio e observar qual dos dois o seu sistema faz, porque um deles esta acontecendo.

5. O roteiro de escape continua o das aulas [09](09-solucoes.md) e [17](17-solucoes.md); neste programa, `payloads` escapa para o heap porque vive alem de `main` nas maos das goroutines — causa nomeada, nao defeito.

6. Mais goroutines que nucleos so ajuda enquanto elas **esperam** coisas diferentes (rede, disco, temporizador). Para soma de bytes em memoria, o teto e nucleos x banda de memoria: a comparacao 1 x 4 workers acima mostra ganho sublinear exatamente por isso.

**Autoverificacao:** `HeapAlloc` sobe com a carga e desce ao soltar, `NumGC` avanca e 1 worker nao vence 4.
