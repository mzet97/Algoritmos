package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func raiz(a, b int) string {
	if a == 0 {
		return "None"
	}
	return bib1.PythonFloat(float64(-b) / float64(a))
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sa, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		sb, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		a, _ := strconv.Atoi(sa)
		b, _ := strconv.Atoi(sb)
		fmt.Println(raiz(a, b))
	}
}
