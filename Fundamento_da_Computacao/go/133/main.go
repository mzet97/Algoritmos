package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func troco(m, q []int) float64 {
	total := 0
	for i := range m { total += m[i] * q[i] }
	return float64(total) / 100.0
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sn, err := bib1.Entrada(scanner)
		if err != nil { break }
		n, _ := strconv.Atoi(sn)
		m := make([]int, n)
		for i := 0; i < n; i++ {
			s, err := bib1.Entrada(scanner)
			if err != nil { return }
			m[i], _ = strconv.Atoi(s)
		}
		q := make([]int, n)
		for i := 0; i < n; i++ {
			s, err := bib1.Entrada(scanner)
			if err != nil { return }
			q[i], _ = strconv.Atoi(s)
		}
		fmt.Println(bib1.PythonFloat(troco(m, q)))
	}
}
