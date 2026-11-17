package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func operacao(x, y int, op string) string {
	switch op {
	case "+":
		return strconv.Itoa(x + y)
	case "-":
		return strconv.Itoa(x - y)
	case "x":
		return strconv.Itoa(x * y)
	case "/":
		return bib1.PythonFloat(float64(x) / float64(y))
	}
	return ""
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
		op, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		x, _ := strconv.Atoi(sx)
		y, _ := strconv.Atoi(sy)
		fmt.Println(operacao(x, y, op))
	}
}
