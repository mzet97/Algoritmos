package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
)

func maiorIgualLista(x, y []int) bool {
	lx := len(x)
	ly := len(y)
	if lx != ly {
		return lx > ly
	}
	for i := lx - 1; i >= 0; i-- {
		if x[i] != y[i] {
			return x[i] > y[i]
		}
	}
	return true
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sx, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		sy, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		xLista := make([]int, 0, len(sx))
		for i := len(sx) - 1; i >= 0; i-- {
			xLista = append(xLista, int(sx[i]-'0'))
		}
		yLista := make([]int, 0, len(sy))
		for i := len(sy) - 1; i >= 0; i-- {
			yLista = append(yLista, int(sy[i]-'0'))
		}
		if maiorIgualLista(xLista, yLista) {
			fmt.Println("1")
		} else {
			fmt.Println("0")
		}
	}
}
