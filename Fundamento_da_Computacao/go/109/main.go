package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
)

func listaParaInt(lst []int) uint64 {
	val := uint64(0)
	for i := len(lst) - 1; i >= 0; i-- {
		val = val*10 + uint64(lst[i])
	}
	return val
}

func intParaLista(val uint64) []int {
	if val == 0 { return []int{0} }
	lst := []int{}
	for val > 0 {
		lst = append(lst, int(val%10))
		val /= 10
	}
	return lst
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
		yStr, err := bib1.Entrada(scanner)
		if err != nil { break }
		xLista := parseReversed(xStr)
		yLista := parseReversed(yStr)
		dx := listaParaInt(xLista)
		dy := listaParaInt(yLista)
		q := dx / dy
		rem := dx % dy
		qList := intParaLista(q)
		rList := intParaLista(rem)
		// reverse
		for i, j := 0, len(qList)-1; i < j; i, j = i+1, j-1 { qList[i], qList[j] = qList[j], qList[i] }
		for i, j := 0, len(rList)-1; i < j; i, j = i+1, j-1 { rList[i], rList[j] = rList[j], rList[i] }
		fmt.Printf("(%s,%s)\n", fmtResult(qList), fmtResult(rList))
	}
}
