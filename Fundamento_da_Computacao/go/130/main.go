package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strings"
)

func subString(L, S string) string {
	if strings.Contains(L, S) {
		return "True"
	}
	return "False"
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 2*1024*1024), 2*1024*1024)
	for {
		L, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		var S string
		if L == "@entradagrande" {
			L = strings.Repeat("ab", 2000)
			S = strings.Repeat("ab", 1000) + "c"
		} else {
			S, err = bib1.Entrada(scanner)
			if err != nil {
				break
			}
		}
		fmt.Println(subString(L, S))
	}
}
