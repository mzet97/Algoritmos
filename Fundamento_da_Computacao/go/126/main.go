package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strings"
)

func remove(L string, c byte) string {
	var sb strings.Builder
	sb.Grow(len(L))
	for i := 0; i < len(L); i++ {
		if L[i] != c {
			sb.WriteByte(L[i])
		}
	}
	return sb.String()
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
			L = strings.Repeat("ca", 500000)
		}
		cStr, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		fmt.Println(remove(L, cStr[0]))
	}
}
