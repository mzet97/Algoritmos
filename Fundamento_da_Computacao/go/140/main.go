package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func perfeito(n int) bool {
	if n < 2 { return false }
	soma := 1
	for i := 2; i*i <= n; i++ {
		if n%i == 0 {
			soma += i
			if i != n/i { soma += n / i }
		}
	}
	return soma == n
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sn, err := bib1.Entrada(scanner)
		if err != nil { break }
		n, _ := strconv.Atoi(sn)
		fmt.Println(perfeito(n))
	}
}
