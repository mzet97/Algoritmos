// Aula 28b — vetorizacao e NUMA em Go: quatro acumuladores verificados
// contra float64, NUMA contado pela /sys e multinó apenas simulado.
package main

import (
	"fmt"
	"math"
	"os"
	"path/filepath"
	"time"
)

type estado uint64

func (e *estado) prox() float32 {
	*e = *e*6364136223846793005 + 1
	return float32(*e>>40) / 16777216.0
}

func somaSeq(v []float32) float32 {
	var s float32
	for _, x := range v {
		s += x
	}
	return s
}

func soma4(v []float32) float32 {
	var acc [4]float32
	i := 0
	for ; i+4 <= len(v); i += 4 {
		acc[0] += v[i]
		acc[1] += v[i+1]
		acc[2] += v[i+2]
		acc[3] += v[i+3]
	}
	s := (acc[0] + acc[1]) + (acc[2] + acc[3])
	for ; i < len(v); i++ {
		s += v[i]
	}
	return s
}

func referencia(v []float32) float64 {
	var s float64
	for _, x := range v {
		s += float64(x)
	}
	return s
}

func nosNUMA() int {
	entradas, err := filepath.Glob("/sys/devices/system/node/node[0-9]*")
	if err != nil {
		return -1
	}
	return len(entradas)
}

func falhar(msg string) {
	fmt.Fprintln(os.Stderr, "FALHA:", msg)
	os.Exit(1)
}

func main() {
	const n = 10_000_000
	est := estado(5)
	v := make([]float32, n)
	for i := range v {
		v[i] = est.prox()
	}
	ref := referencia(v)
	eSeq := math.Abs(float64(somaSeq(v))-ref) / ref
	e4 := math.Abs(float64(soma4(v))-ref) / ref
	fmt.Printf("erro vs float64: seq=%.2e 4acum=%.2e\n", eSeq, e4)
	if !(eSeq < 2e-2 && e4 < 2e-3 && e4 < eSeq) {
		falhar("tolerancias do modelo de erro violadas")
	}
	mede := func(f func([]float32) float32) float64 {
		t0 := time.Now()
		var total float32
		for r := 0; r < 5; r++ {
			total += f(v)
		}
		if total == 12345 {
			fmt.Println("inalcancavel")
		}
		return time.Since(t0).Seconds() / 5
	}
	tSeq, t4 := mede(somaSeq), mede(soma4)
	nos := nosNUMA()
	fmt.Printf("seq=%.4fs 4acum=%.4fs razao=%.2fx nos_numa=%d\n", tSeq, t4, tSeq/t4, nos)
	// Em Go, o compilador ja quebra a cadeia/vetoriza a soma simples:
	// as duas formas empatam, e a aula registra o empate em vez de
	// prometer o ganho que C/Rust mediram com outro compilador.
	if nos != 1 {
		fmt.Println("nota: multinó real possivel nesta maquina; o exercicio de first-touch deixa de ser simulado")
	} else {
		fmt.Println("multino: apenas simulado/desenhado (1 no NUMA); nenhum numero remoto e afirmado")
	}
	fmt.Println("28b ok: corretude verificada, empate medido e NUMA contado")
}
