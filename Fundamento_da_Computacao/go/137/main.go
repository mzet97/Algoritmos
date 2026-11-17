package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
	"strings"
)

func transposta(m [][]int, n, mm int) [][]int {
	r := make([][]int, mm)
	for j := 0; j < mm; j++ {
		r[j] = make([]int, n)
		for i := 0; i < n; i++ {
			r[j][i] = m[i][j]
		}
	}
	return r
}

func fmtMatrix(m [][]int) string {
	parts := []string{}
	for _, row := range m {
		parts = append(parts, fmt.Sprintf("%v", row))
	}
	return "[" + strings.Join(parts, ", ") + "]"
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 2*1024*1024), 2*1024*1024)
	for {
		sn, err := bib1.Entrada(scanner)
		if err != nil { break }
		n, _ := strconv.Atoi(sn)
		var mm int
		var m [][]int
		if n > -1 {
			sm, err := bib1.Entrada(scanner)
			if err != nil { break }
			mm, _ = strconv.Atoi(sm)
			m = make([][]int, n)
			for i := 0; i < n; i++ {
				m[i] = make([]int, mm)
				for j := 0; j < mm; j++ {
					s, err := bib1.Entrada(scanner)
					if err != nil { return }
					m[i][j], _ = strconv.Atoi(s)
				}
			}
		} else {
			n, mm = 1000, 1000
			m = make([][]int, n)
			for i := 0; i < n; i++ {
				m[i] = make([]int, mm)
				for j := 0; j < mm; j++ { m[i][j] = i*n + j }
			}
		}
		fmt.Println(fmtMatrix(transposta(m, n, mm)))
	}
}
