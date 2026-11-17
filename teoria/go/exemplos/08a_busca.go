package main

import (
	"fmt"
	"math/rand"
	"sort"
	"time"
)

// BuscaBinaria devolve (indice, true) se alvo esta em valores, ou (0, false).
//
// Pre-condicao: valores ordenada em ordem nao decrescente.
// Invariante: se alvo esta na fatia, esta em valores[ini:fim].
func BuscaBinaria(valores []int, alvo int) (int, bool) {
	ini, fim := 0, len(valores)
	for ini < fim {
		meio := ini + (fim-ini)/2
		switch {
		case valores[meio] == alvo:
			return meio, true
		case alvo < valores[meio]:
			fim = meio
		default:
			ini = meio + 1
		}
	}
	return 0, false
}

// BuscaLinear e a referencia simples: percorre tudo, sem exigir ordenacao.
func BuscaLinear(valores []int, alvo int) (int, bool) {
	for i, v := range valores {
		if v == alvo {
			return i, true
		}
	}
	return 0, false
}

// checarPropriedade confere o CONTRATO (nao a igualdade com a referencia):
// com repetidos, dois indices diferentes podem ser ambos corretos.
func checarPropriedade(tentativas int) {
	rng := rand.New(rand.NewSource(42)) // semente fixa: a falha e reproduzivel
	for t := 0; t < tentativas; t++ {
		n := rng.Intn(51)
		valores := make([]int, n)
		for i := range valores {
			valores[i] = rng.Intn(201) - 100
		}
		sort.Ints(valores)
		alvo := rng.Intn(221) - 110
		idx, ok := BuscaBinaria(valores, alvo)
		tem := sort.SearchInts(valores, alvo) < len(valores) && valores[sort.SearchInts(valores, alvo)] == alvo
		if tem {
			if !ok || valores[idx] != alvo {
				panic(fmt.Sprintf("tentativa %d: indice invalido", t))
			}
		} else if ok {
			panic(fmt.Sprintf("tentativa %d: falso positivo", t))
		}
	}
}

func medir(repeticoes int, f func()) float64 {
	inicio := time.Now()
	for i := 0; i < repeticoes; i++ {
		f()
	}
	return time.Since(inicio).Seconds() / float64(repeticoes)
}

func main() {
	casos := []struct {
		nome    string
		valores []int
		alvo    int
		querOk  bool
	}{
		{"vazia", nil, 5, false},
		{"unitario acha", []int{5}, 5, true},
		{"unitario falta", []int{5}, 4, false},
		{"inicio", []int{1, 3, 5, 7}, 1, true},
		{"fim", []int{1, 3, 5, 7}, 7, true},
		{"ausente", []int{1, 3, 5, 7}, 4, false},
		{"repetidos", []int{2, 2, 2}, 2, true},
	}
	for _, c := range casos {
		idx, ok := BuscaBinaria(c.valores, c.alvo)
		if ok != c.querOk || (ok && c.valores[idx] != c.alvo) {
			panic("caso " + c.nome + ": contrato violado")
		}
	}

	checarPropriedade(2000)

	grande := make([]int, 1_000_000)
	for i := range grande {
		grande[i] = i * 2
	}
	for _, alvo := range []int{1_999_998, 1_999_999} {
		if _, ok := BuscaBinaria(grande, alvo); ok != (alvo == 1_999_998) {
			panic("grande: contrato violado")
		}
		if _, ok := BuscaLinear(grande, alvo); ok != (alvo == 1_999_998) {
			panic("grande: referencia violada")
		}
	}

	tLin := medir(5, func() { BuscaLinear(grande, 1_999_999) })
	tBin := medir(200, func() { BuscaBinaria(grande, 1_999_999) })
	fmt.Printf("linear: %.6fs/media (5x)\n", tLin)
	fmt.Printf("binaria: %.9fs/media (200x)\n", tBin)
}
