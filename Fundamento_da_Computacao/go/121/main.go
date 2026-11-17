package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func gcd(a, b int64) int64 {
	if a < 0 {
		a = -a
	}
	if b < 0 {
		b = -b
	}
	for b != 0 {
		a, b = b, a%b
	}
	return a
}

func reduz(p, q int64) (int64, int64) {
	g := gcd(p, q)
	return p / g, q / g
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sp, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		sq, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		p, _ := strconv.ParseInt(sp, 10, 64)
		q, _ := strconv.ParseInt(sq, 10, 64)
		rp, rq := reduz(p, q)
		fmt.Printf("(%d, %d)\n", rp, rq)
	}
}
