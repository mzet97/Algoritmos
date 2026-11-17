package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"math"
	"os"
	"strconv"
)

func raiz(a, b, c float64) (interface{}, interface{}) {
	delta := b*b - 4*a*c
	if delta < 0 { return nil, nil }
	r1 := (-b + math.Sqrt(delta)) / (2 * a)
	if delta == 0 { return r1, nil }
	r2 := (-b - math.Sqrt(delta)) / (2 * a)
	return r1, r2
}

func fmtFloat(v interface{}) string {
	if v == nil { return "None" }
	f := v.(float64)
	if f == float64(int(f)) && math.Abs(f) < 1e15 { return strconv.Itoa(int(f)) }
	return strconv.FormatFloat(f, 'f', -1, 64)
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sa, err := bib1.Entrada(scanner)
		if err != nil { break }
		sb, err := bib1.Entrada(scanner)
		if err != nil { break }
		sc, err := bib1.Entrada(scanner)
		if err != nil { break }
		a, _ := strconv.ParseFloat(sa, 64)
		b, _ := strconv.ParseFloat(sb, 64)
		c, _ := strconv.ParseFloat(sc, 64)
		r1, r2 := raiz(a, b, c)
		if r1 == nil {
			fmt.Println("None")
		} else if r2 == nil {
			fmt.Printf("(%s, None)\n", fmtFloat(r1))
		} else {
			fmt.Printf("(%s, %s)\n", fmtFloat(r1), fmtFloat(r2))
		}
	}
}
