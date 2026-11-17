package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strings"
)

func ehPalindromo(L string) string {
	// Remove spaces
	s := strings.ReplaceAll(L, " ", "")
	n := len(s)
	for i := 0; i < n/2; i++ {
		if s[i] != s[n-1-i] {
			return "False"
		}
	}
	return "True"
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 2*1024*1024), 2*1024*1024)
	for {
		L, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		if L == "@entradagrande1" {
			L = strings.Repeat("ab", 500000) + "a"
		} else if L == "@entradagrande2" {
			L = strings.Repeat("ab", 500000)
		}
		fmt.Println(ehPalindromo(L))
	}
}
