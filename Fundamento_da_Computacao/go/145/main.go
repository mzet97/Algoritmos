package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func serie(N int) float64 {
	sum := 0.0
	for i := 1; i <= N; i++ {
		sum += float64(i) / float64(N-i+1)
	}
	return sum
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		s, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		n, _ := strconv.Atoi(s)
		fmt.Println(bib1.PythonFloat(serie(n)))
	}
}
