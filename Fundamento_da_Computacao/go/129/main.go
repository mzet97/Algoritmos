package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strings"
)

func reverte(L string) string {
	n := len(L)
	b := make([]byte, n)
	for i := 0; i < n; i++ {
		b[i] = L[n-1-i]
	}
	return string(b)
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 2*1024*1024), 2*1024*1024)
	for {
		L, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		if L == "@entradagrande" {
			L = strings.Repeat("ab", 500000)
		}
		fmt.Println(reverte(L))
	}
}
