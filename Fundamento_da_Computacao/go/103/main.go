package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func calcSec(h1, m1, h2, m2 int) int {
	s1 := h1*3600 + m1*60
	s2 := h2*3600 + m2*60
	diff := s2 - s1
	if diff < 0 {
		diff = -diff
	}
	return diff
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sh1, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		sm1, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		sh2, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		sm2, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		h1, _ := strconv.Atoi(sh1)
		m1, _ := strconv.Atoi(sm1)
		h2, _ := strconv.Atoi(sh2)
		m2, _ := strconv.Atoi(sm2)
		fmt.Println(calcSec(h1, m1, h2, m2))
	}
}
