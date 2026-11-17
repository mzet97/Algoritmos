package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func doisHorarios(h, m int) (string, string) {
	minutos := m * 5
	if m == 12 {
		minutos = 0
	}
	hora := h % 12
	h1 := fmt.Sprintf("%02d:%02d", hora, minutos)
	h2 := fmt.Sprintf("%02d:%02d", hora+12, minutos)
	return h1, h2
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sh, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		sm, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		h, _ := strconv.Atoi(sh)
		m, _ := strconv.Atoi(sm)
		h1, h2 := doisHorarios(h, m)
		fmt.Printf("%s / %s\n", h1, h2)
	}
}
