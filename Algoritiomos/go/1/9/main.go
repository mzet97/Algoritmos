package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
)

func RepresentacaoDecimal(n int64) []int {
	if n == 0 {
		return []int{0}
	}
	var digitos []int
	for n > 0 {
		digitos = append(digitos, int(n%10))
		n /= 10
	}
	// Inverter
	for i, j := 0, len(digitos)-1; i < j; i, j = i+1, j-1 {
		digitos[i], digitos[j] = digitos[j], digitos[i]
	}
	return digitos
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for scanner.Scan() {
		line := scanner.Text()
		if line == "" {
			continue
		}
		n, err := strconv.ParseInt(line, 10, 64)
		if err != nil {
			continue
		}
		R := RepresentacaoDecimal(n)
		for i, d := range R {
			if i > 0 {
				fmt.Printf(" ")
			}
			fmt.Printf("%d", d)
		}
		fmt.Println()
	}
}
