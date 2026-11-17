package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"math"
	"os"
	"strconv"
)

func ehPrimo(x int64) bool {
	if x < 2 {
		return false
	}
	if x == 2 {
		return true
	}
	if x%2 == 0 {
		return false
	}
	limit := int64(math.Sqrt(float64(x)))
	// Adjust for floating point imprecision
	for (limit+1)*(limit+1) <= x {
		limit++
	}
	for i := int64(3); i <= limit; i += 2 {
		if x%i == 0 {
			return false
		}
	}
	return true
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		s, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		x, _ := strconv.ParseInt(s, 10, 64)
		if ehPrimo(x) {
			fmt.Println("True")
		} else {
			fmt.Println("False")
		}
	}
}
