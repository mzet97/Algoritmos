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
		x, _ := strconv.Atoi(sx)
		y, _ := strconv.Atoi(sy)
		fmt.Println(mdc(x, y))
	}
}
