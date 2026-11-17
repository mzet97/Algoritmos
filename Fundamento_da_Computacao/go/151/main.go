package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
	"strings"
)

func multiMatriz(r, s, t int, m1, m2 [][]int) [][]int {
	mr := make([][]int, r)
	for i := 0; i < r; i++ {
		mr[i] = make([]int, t)
		for j := 0; j < t; j++ {
			for k := 0; k < s; k++ {
				mr[i][j] += m1[i][k] * m2[k][j]
			}
		}
	}
	return mr
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sr, err := bib1.Entrada(scanner)
		if err != nil { break }
		ss, err := bib1.Entrada(scanner)
		if err != nil { break }
		st, err := bib1.Entrada(scanner)
		if err != nil { break }
		r, _ := strconv.Atoi(sr)
		s, _ := strconv.Atoi(ss)
		t, _ := strconv.Atoi(st)
		m1 := make([][]int, r)
		for i := 0; i < r; i++ {
			line, err := bib1.Entrada(scanner)
			if err != nil { return }
			parts := strings.Fields(line)
			m1[i] = make([]int, s)
			for j := 0; j < s; j++ { m1[i][j], _ = strconv.Atoi(parts[j]) }
		}
		m2 := make([][]int, s)
		for i := 0; i < s; i++ {
			line, err := bib1.Entrada(scanner)
			if err != nil { return }
			parts := strings.Fields(line)
			m2[i] = make([]int, t)
			for j := 0; j < t; j++ { m2[i][j], _ = strconv.Atoi(parts[j]) }
		}
		mr := multiMatriz(r, s, t, m1, m2)
		for i := 0; i < r; i++ {
			if i > 0 { fmt.Print(" | ") }
			for j := 0; j < t; j++ {
				if j > 0 { fmt.Print(" ") }
				fmt.Print(mr[i][j])
			}
		}
		fmt.Println()
	}
}
