package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strings"
)

func subtraiLista(x, y []int) []int {
	resultado := []int{}
	borrow := 0
	for i := 0; i < len(x); i++ {
		a := x[i]
		b := 0
		if i < len(y) { b = y[i] }
		a -= borrow
		if a < b { a += 10; borrow = 1 } else { borrow = 0 }
		resultado = append(resultado, a-b)
	}
	for len(resultado) > 1 && resultado[len(resultado)-1] == 0 {
		resultado = resultado[:len(resultado)-1]
	}
	return resultado
}

func parseReversed(s string) []int {
	r := []int{}
	for i := len(s) - 1; i >= 0; i-- { r = append(r, int(s[i]-'0')) }
	return r
}

func fmtResult(r []int) string {
	s := make([]byte, len(r))
	for i, d := range r { s[i] = byte('0' + d) }
	return string(s)
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 2*1024*1024), 2*1024*1024)
	for {
		xStr, err := bib1.Entrada(scanner)
		if err != nil { break }
		var yStr string
		if xStr == "@entradagrande1" {
			xStr = strings.Repeat("1", 1000000)
			yStr = strings.Repeat("9", 1000000-1)
		} else if xStr == "@entradagrande2" {
			xStr = strings.Repeat("1", 1000000)
			yStr = strings.Repeat("1", 1000000)
		} else {
			yStr, err = bib1.Entrada(scanner)
			if err != nil { break }
		}
		xLista := parseReversed(xStr)
		yLista := parseReversed(yStr)
		r := subtraiLista(xLista, yLista)
		for i, j := 0, len(r)-1; i < j; i, j = i+1, j-1 { r[i], r[j] = r[j], r[i] }
		fmt.Println(fmtResult(r))
	}
}
