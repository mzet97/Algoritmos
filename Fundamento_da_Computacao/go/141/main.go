package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func notas(m []int, n []float64) ([]int, []int) {
	maxN, minN := n[0], n[0]
	for _, v := range n {
		if v > maxN { maxN = v }
		if v < minN { minN = v }
	}
	maiores := []int{}
	menores := []int{}
	for i := range m {
		if n[i] == maxN { maiores = append(maiores, m[i]) }
		if n[i] == minN { menores = append(menores, m[i]) }
	}
	return maiores, menores
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sn, err := bib1.Entrada(scanner)
		if err != nil { break }
		nVal, _ := strconv.Atoi(sn)
		var m []int
		var notasVec []float64
		if nVal >= 0 {
			n := nVal
			m = make([]int, n)
			for i := 0; i < n; i++ {
				s, err := bib1.Entrada(scanner)
				if err != nil { return }
				m[i], _ = strconv.Atoi(s)
			}
			notasVec = make([]float64, n)
			for i := 0; i < n; i++ {
				s, err := bib1.Entrada(scanner)
				if err != nil { return }
				notasVec[i], _ = strconv.ParseFloat(s, 64)
			}
		} else {
			m = make([]int, 1000000)
			for i := range m { m[i] = i }
			notasVec = make([]float64, 1000000)
			for i := 0; i < 999998; i++ { notasVec[i] = 2.0 }
			notasVec[999998] = 1.0
			notasVec[999999] = 3.0
		}
		maiores, menores := notas(m, notasVec)
		fmt.Printf("(%v, %v)\n", maiores, menores)
	}
}
