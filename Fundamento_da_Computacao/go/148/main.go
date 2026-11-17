package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func fatorial(n int) int {
	r := 1
	for i := 2; i <= n; i++ { r *= i }
	return r
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sn, err := bib1.Entrada(scanner)
		if err != nil { break }
		n, _ := strconv.Atoi(sn)
		fmt.Println(fatorial(n))
	}
}
