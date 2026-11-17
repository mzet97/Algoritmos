package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strings"
)

func somaLista(x, y []int) []int {
	resultado := []int{}
	carry := 0
	length := len(x)
	if len(y) > length { length = len(y) }
	for i := 0; i < length; i++ {
		a, b := 0, 0
		if i < len(x) { a = x[i] }
		if i < len(y) { b = y[i] }
		s := a + b + carry
		resultado = append(resultado, s%10)
		carry = s / 10
	}
	if carry > 0 { resultado = append(resultado, carry) }
	return resultado
}

func parseReversed(s string) []int {
	r := []int{}
	for i := len(s) - 1; i >= 0; i-- {
		d := int(s[i] - '0')
		r = append(r, d)
	}
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
			xStr = strings.Repeat("9", 1000000)
			yStr = strings.Repeat("9", 1000000)
		}
		xLista := parseReversed(xStr)
		yLista := parseReversed(yStr)
		r := somaLista(xLista, yLista)
		// reverse
		for i, j := 0, len(r)-1; i < j; i, j = i+1, j-1 { r[i], r[j] = r[j], r[i] }
		fmt.Println(fmtResult(r))
	}
}
