package main

import (
	"fmt"
	"runtime"
	"sync"
	"sync/atomic"
	"time"
)

const n = 20_000_000
const repeticoes = 5

// medir devolve o minimo de N rodadas: filtra ruido para baixo.
func medir(repeticoes int, f func() int64) (int64, float64) {
	var melhor float64 = 1e30
	var resultado int64
	for i := 0; i < repeticoes; i++ {
		inicio := time.Now()
		resultado = f()
		if dt := time.Since(inicio).Seconds(); dt < melhor {
			melhor = dt
		}
	}
	return resultado, melhor
}

func somaSeq(v []int64) int64 {
	var s int64
	for _, x := range v {
		s += x
	}
	return s
}

func somaParalela(v []int64, workers int) int64 {
	parciais := make([]int64, workers)
	var wg sync.WaitGroup
	for w := 0; w < workers; w++ {
		wg.Add(1)
		go func(w int) {
			defer wg.Done()
			ini := (len(v) * w) / workers
			fim := (len(v) * (w + 1)) / workers
			var s int64
			for _, x := range v[ini:fim] {
				s += x
			}
			parciais[w] = s
		}(w)
	}
	wg.Wait()
	var total int64
	for _, p := range parciais {
		total += p
	}
	return total
}

// somaContendida: todas as goroutines no MESMO atomico (correto e lento).
func somaContendida(v []int64, workers int) int64 {
	var total atomic.Int64
	var wg sync.WaitGroup
	for w := 0; w < workers; w++ {
		wg.Add(1)
		go func(w int) {
			defer wg.Done()
			ini := (len(v) * w) / workers
			fim := (len(v) * (w + 1)) / workers
			for _, x := range v[ini:fim] {
				total.Add(x)
			}
		}(w)
	}
	wg.Wait()
	return total.Load()
}

func main() {
	fmt.Printf("n=%d gcpus=%d gomaxprocs=%d\n", n, runtime.NumCPU(), runtime.GOMAXPROCS(0))
	v := make([]int64, n)
	for i := range v {
		v[i] = int64(i % 97)
	}
	esperado := somaSeq(v)

	// Aquecimento fora da conta.
	_ = somaParalela(v, 2)

	tot, tSeq := medir(repeticoes, func() int64 { return somaSeq(v) })
	if tot != esperado {
		panic("baseline incorreto")
	}
	fmt.Printf("seq: %.3fs\n", tSeq)

	for _, w := range []int{2, 4, 8} {
		tot, t := medir(repeticoes, func() int64 { return somaParalela(v, w) })
		if tot != esperado {
			panic("variante incorreta")
		}
		fmt.Printf("par%d: %.3fs speedup=%.2fx\n", w, t, tSeq/t)
	}

	tot, tCon := medir(repeticoes, func() int64 { return somaContendida(v, 4) })
	if tot != esperado {
		panic("contendido incorreto")
	}
	fmt.Printf("contendido4: %.3fs (correto, mas serializado pela linha de cache)\n", tCon)

	// Conta do acelerador: aritmetica deterministica, sem hardware.
	const bytesTotais = float64(n * 8)
	const bandaGBs = 16.0
	const tKernel = 0.001
	tViagem := 2 * (bytesTotais / (bandaGBs * 1024 * 1024 * 1024))
	fmt.Printf("viagem=%.4fs kernel=%.3fs total=%.4fs\n", tViagem, tKernel, tViagem+tKernel)
	if tViagem+tKernel < tSeq {
		fmt.Println("veredito: a GPU hipotetica venceria NESTE cenario")
	} else {
		fmt.Println("veredito: fique na CPU; a viagem custa mais que o calculo")
	}
	fmt.Println("hpc ok: baseline verificado, escala medida, conta feita")
}
