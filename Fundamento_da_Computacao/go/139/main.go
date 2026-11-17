package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func pg(a, q, n int) []int {
	resultado := make([]int, n)
	val := a
	for i := 0; i < n; i++ {
		resultado[i] = val
		val *= q
	}
	return resultado
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sa, err := bib1.Entrada(scanner)
		if err != nil { break }
		sq, err := bib1.Entrada(scanner)
		if err != nil { break }
		sn, err := bib1.Entrada(scanner)
		if err != nil { break }
		a, _ := strconv.Atoi(sa)
		q, _ := strconv.Atoi(sq)
		n, _ := strconv.Atoi(sn)
		fmt.Println(pg(a, q, n))
	}
}
