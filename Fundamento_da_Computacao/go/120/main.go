package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func mdc(x, y int) int {
	for y != 0 {
		x, y = y, x%y
	}
	return x
}

func mmc(x, y int) int {
	return x / mdc(x, y) * y
}

func multiplos(a, b, n int) int {
	lcm := mmc(a, b)
	if lcm >= n {
		return 0
	}
	return (n - 1) / lcm
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sa, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		sb, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		sn, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		a, _ := strconv.Atoi(sa)
		b, _ := strconv.Atoi(sb)
		n, _ := strconv.Atoi(sn)
		fmt.Println(multiplos(a, b, n))
	}
}
