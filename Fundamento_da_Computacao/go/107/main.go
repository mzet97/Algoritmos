package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func multiplicaLista(x []int, y int) []int {
	resultado := []int{}
	carry := 0
	for _, d := range x {
		p := d*y + carry
		resultado = append(resultado, p%10)
		carry = p / 10
	}
	for carry > 0 {
		resultado = append(resultado, carry%10)
		carry /= 10
	}
	if len(resultado) == 0 { resultado = []int{0} }
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
	for {
		xStr, err := bib1.Entrada(scanner)
		if err != nil { break }
		sy, err := bib1.Entrada(scanner)
		if err != nil { break }
		y, _ := strconv.Atoi(sy)
		xLista := parseReversed(xStr)
		r := multiplicaLista(xLista, y)
		for i, j := 0, len(r)-1; i < j; i, j = i+1, j-1 { r[i], r[j] = r[j], r[i] }
		fmt.Println(fmtResult(r))
	}
}
