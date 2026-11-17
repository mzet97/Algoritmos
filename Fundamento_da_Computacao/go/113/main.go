package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func contaNum(lista []int) (int, int) {
	qp, qn := 0, 0
	for _, x := range lista {
		if x > 0 { qp++ }
		if x < 0 { qn++ }
	}
	return qp, qn
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sn, err := bib1.Entrada(scanner)
		if err != nil { break }
		n, _ := strconv.Atoi(sn)
		lista := make([]int, n)
		for i := 0; i < n; i++ {
			s, err := bib1.Entrada(scanner)
			if err != nil { return }
			lista[i], _ = strconv.Atoi(s)
		}
		qp, qn := contaNum(lista)
		fmt.Printf("(%d, %d)\n", qp, qn)
	}
}
