package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func contagem(x int, y int) int {
	s := strconv.Itoa(abs(x))
	target := strconv.Itoa(abs(y) % 10)[0]
	count := 0
	for i := 0; i < len(s); i++ {
		if s[i] == target {
			count++
		}
	}
	return count
}

func abs(x int) int {
	if x < 0 { return -x }
	return x
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sx, err := bib1.Entrada(scanner)
		if err != nil { break }
		sy, err := bib1.Entrada(scanner)
		if err != nil { break }
		x, _ := strconv.Atoi(sx)
		y, _ := strconv.Atoi(sy)
		fmt.Println(contagem(x, y))
	}
}
