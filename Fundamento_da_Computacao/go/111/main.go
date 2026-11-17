package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func calcDist(v, a, t float64) float64 {
	return v*t + 0.5*a*t*t
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sv, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		sa, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		st, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		v, _ := strconv.ParseFloat(sv, 64)
		a, _ := strconv.ParseFloat(sa, 64)
		t, _ := strconv.ParseFloat(st, 64)
		fmt.Println(bib1.PythonFloat(calcDist(v, a, t)))
	}
}
