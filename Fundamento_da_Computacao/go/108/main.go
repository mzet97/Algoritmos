package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strings"
)

func multiplicaListaLista(x, y []int) []int {
	resultado := make([]int, len(x)+len(y))
	for i := range x {
		carry := 0
		for j := range y {
			p := resultado[i+j] + x[i]*y[j] + carry
			resultado[i+j] = p % 10
			carry = p / 10
		}
		k := i + len(y)
		for carry > 0 {
			p := resultado[k] + carry
			resultado[k] = p % 10
			carry = p / 10
			k++
		}
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
		if xStr != "@entradagrande" {
			yStr, err = bib1.Entrada(scanner)
			if err != nil { break }
		} else {
			xStr = strings.Repeat("9", 1000)
			yStr = strings.Repeat("9", 1000)
		}
		xLista := parseReversed(xStr)
		yLista := parseReversed(yStr)
		r := multiplicaListaLista(xLista, yLista)
		for i, j := 0, len(r)-1; i < j; i, j = i+1, j-1 { r[i], r[j] = r[j], r[i] }
		fmt.Println(fmtResult(r))
	}
}
