package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"math"
	"os"
	"strconv"
)

func pi(N int) float64 {
	S := 0.0
	for i := 0; i < N; i++ {
		denom := float64(2*i + 1)
		term := 1.0 / (denom * denom * denom)
		if i%2 == 0 {
			S += term
		} else {
			S -= term
		}
	}
	return math.Cbrt(32.0 * S)
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		s, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		n, _ := strconv.Atoi(s)
		fmt.Printf("%.10f\n", pi(n))
	}
}
