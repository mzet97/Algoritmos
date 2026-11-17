package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"math"
	"os"
	"strconv"
)

func pontoReta(a, b float64, ponto [2]float64) string {
	x, y := ponto[0], ponto[1]
	yReta := a*x + b
	diff := y - yReta
	if math.Abs(diff) <= 0.00001 { return "=" }
	if diff > 0 { return ">" }
	return "<"
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sa, err := bib1.Entrada(scanner)
		if err != nil { break }
		sb, err := bib1.Entrada(scanner)
		if err != nil { break }
		sx, err := bib1.Entrada(scanner)
		if err != nil { break }
		sy, err := bib1.Entrada(scanner)
		if err != nil { break }
		a, _ := strconv.ParseFloat(sa, 64)
		b, _ := strconv.ParseFloat(sb, 64)
		x, _ := strconv.ParseFloat(sx, 64)
		y, _ := strconv.ParseFloat(sy, 64)
		fmt.Println(pontoReta(a, b, [2]float64{x, y}))
	}
}
