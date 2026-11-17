package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strings"
)

func isVowel(c byte) bool {
	switch c {
	case 'a', 'e', 'i', 'o', 'u', 'A', 'E', 'I', 'O', 'U':
		return true
	}
	return false
}

func duplica(L string) string {
	var sb strings.Builder
	sb.Grow(len(L) * 2)
	for i := 0; i < len(L); i++ {
		sb.WriteByte(L[i])
		if isVowel(L[i]) {
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
			L = strings.Repeat("a", 500000)
		}
		fmt.Println(duplica(L))
	}
}
