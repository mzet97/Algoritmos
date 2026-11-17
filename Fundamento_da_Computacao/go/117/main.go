package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func cientifico(n float64) string {
	if n == 0 { return "0.0x10^0" }
	sinal := ""
	if n < 0 { sinal = "-"; n = -n }
	exp := 0
	for n >= 10 { n /= 10; exp++ }
	for n < 1 { n *= 10; exp-- }
	return fmt.Sprintf("%s%.1fx10^%d", sinal, n, exp)
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sn, err := bib1.Entrada(scanner)
		if err != nil { break }
		n, _ := strconv.ParseFloat(sn, 64)
		fmt.Println(cientifico(n))
	}
}
