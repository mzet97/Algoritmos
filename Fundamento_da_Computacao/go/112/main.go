package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func converte(x int, u1, u2 string) int {
	unidades := map[string]float64{
		"km": 1000, "hm": 100, "dam": 10, "m": 1, "dm": 0.1, "cm": 0.01, "mm": 0.001,
		"kg": 1000, "hg": 100, "dag": 10, "g": 1, "dg": 0.1, "cg": 0.01, "mg": 0.001,
		"kl": 1000, "hl": 100, "dal": 10, "l": 1, "dl": 0.1, "cl": 0.01, "ml": 0.001,
	}
	return int(float64(x) * unidades[u1] / unidades[u2])
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sx, err := bib1.Entrada(scanner)
		if err != nil { break }
		u1, err := bib1.Entrada(scanner)
		if err != nil { break }
		u2, err := bib1.Entrada(scanner)
		if err != nil { break }
		x, _ := strconv.Atoi(sx)
		fmt.Println(converte(x, u1, u2))
	}
}
