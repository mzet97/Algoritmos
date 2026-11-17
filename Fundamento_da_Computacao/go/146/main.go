package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func fibonacci(n int) int {
	if n <= 0 { return 0 }
	if n <= 2 { return 1 }
	a, b := 1, 1
	for i := 2; i < n; i++ { a, b = b, a+b }
	return b
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sn, err := bib1.Entrada(scanner)
		if err != nil { break }
		n, _ := strconv.Atoi(sn)
		fmt.Println(fibonacci(n))
	}
}
