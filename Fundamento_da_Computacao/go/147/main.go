package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
	"strings"
)

func escreverSeqFibonacci(n int) {
	fibs := make([]int, n)
	a, b := 1, 1
	for i := 0; i < n; i++ {
		fibs[i] = a
		a, b = b, a+b
	}
	for k := 1; k < n; k++ {
		parts := make([]string, k)
		for i := 0; i < k; i++ { parts[i] = strconv.Itoa(fibs[i]) }
		fmt.Println(strings.Join(parts, " "))
	}
	parts := make([]string, n)
	for i := 0; i < n; i++ { parts[i] = strconv.Itoa(fibs[i]) }
	fmt.Println(strings.Join(parts, " "))
	for k := n - 1; k >= 1; k-- {
		parts := make([]string, k)
		for i := 0; i < k; i++ { parts[i] = strconv.Itoa(fibs[i]) }
		fmt.Println(strings.Join(parts, " "))
	}
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sn, err := bib1.Entrada(scanner)
		if err != nil { break }
		n, _ := strconv.Atoi(sn)
		escreverSeqFibonacci(n)
	}
}
