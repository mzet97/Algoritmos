package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func raiz(n int64) int64 {
	var i int64
	for i*i <= n {
		i++
	}
	return i - 1
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		s, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		n, _ := strconv.ParseInt(s, 10, 64)
		fmt.Println(raiz(n))
	}
}
