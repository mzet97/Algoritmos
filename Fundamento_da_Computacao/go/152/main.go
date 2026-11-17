package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
	"strings"
)

func sistema(a [][]int, b []int, n int) []float64 {
	x := make([]float64, n)
	for i := 0; i < n; i++ {
		s := float64(b[i])
		for j := 0; j < i; j++ { s -= float64(a[i][j]) * x[j] }
		x[i] = s / float64(a[i][i])
	}
	return x
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sn, err := bib1.Entrada(scanner)
		if err != nil { break }
		nVal, _ := strconv.Atoi(sn)
		var a [][]int
		var b []int
		var n int
		if nVal >= 1 {
			n = nVal
			a = make([][]int, n)
			b = make([]int, n)
			for i := 0; i < n; i++ {
				line, err := bib1.Entrada(scanner)
				if err != nil { return }
				parts := strings.Fields(line)
				a[i] = make([]int, n)
				for j := 0; j < n; j++ { a[i][j], _ = strconv.Atoi(parts[j]) }
				b[i], _ = strconv.Atoi(parts[n])
			}
		} else {
			n = 1000
			a = make([][]int, n)
			for i := 0; i < n; i++ {
				a[i] = make([]int, i+1)
				for j := 0; j <= i; j++ { a[i][j] = 1 }
			}
			b = make([]int, n)
			for i := 0; i < n; i++ { b[i] = 1 }
		}
		x := sistema(a, b, n)
		parts := make([]string, n)
		for i := 0; i < n; i++ { parts[i] = fmt.Sprintf("%.1f", x[i]) }
		fmt.Println(strings.Join(parts, " "))
	}
}
