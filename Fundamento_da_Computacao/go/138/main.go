package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func pa(a, r int, n int) []int {
	resultado := make([]int, n)
	for i := 0; i < n; i++ { resultado[i] = a + i*r }
	return resultado
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sa, err := bib1.Entrada(scanner)
		if err != nil { break }
		sr, err := bib1.Entrada(scanner)
		if err != nil { break }
		sn, err := bib1.Entrada(scanner)
		if err != nil { break }
		a, _ := strconv.Atoi(sa)
		r, _ := strconv.Atoi(sr)
		n, _ := strconv.Atoi(sn)
		fmt.Println(pa(a, r, n))
	}
}
