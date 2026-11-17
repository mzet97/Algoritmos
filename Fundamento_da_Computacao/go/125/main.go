package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
)

func contar(L string, c byte) int {
	count := 0
	for i := 0; i < len(L); i++ {
		if L[i] == c {
			count++
		}
	}
	return count
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 2*1024*1024), 2*1024*1024)
	for {
		L, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		cStr, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		fmt.Println(contar(L, cStr[0]))
	}
}
