package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func saque(s int) [][2]int {
	notas := []int{200, 100, 50, 20, 10, 5}
	resultado := [][2]int{}
	restante := s
	for _, n := range notas {
		if restante >= n {
			q := restante / n
			resultado = append(resultado, [2]int{q, n})
			restante -= q * n
		}
	}
	if restante != 0 { return [][2]int{} }
	return resultado
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		ss, err := bib1.Entrada(scanner)
		if err != nil { break }
		s, _ := strconv.Atoi(ss)
		r := saque(s)
		parts := make([]string, len(r))
		for i, pair := range r {
			parts[i] = fmt.Sprintf("(%d, %d)", pair[0], pair[1])
		}
		fmt.Printf("[%s]\n", fmt.Sprintf("%s", fmt.Sprintf("%v", r)))
	}
}
