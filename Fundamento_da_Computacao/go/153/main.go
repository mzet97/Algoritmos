package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func raiz(n, inf, sup int) int {
	for sup-inf > 1 {
		mid := (inf + sup) / 2
		if mid*mid <= n { inf = mid } else { sup = mid }
	}
	return inf
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sn, err := bib1.Entrada(scanner)
		if err != nil { break }
		n, _ := strconv.Atoi(sn)
		fmt.Println(raiz(n, 0, n+1))
	}
}
