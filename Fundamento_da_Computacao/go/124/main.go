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

func divide(p1, q1, p2, q2 int64) (int64, int64) {
	p := p1 * q2
	q := q1 * p2
	return reduz(p, q)
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sp1, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		sq1, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		sp2, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		sq2, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		p1, _ := strconv.ParseInt(sp1, 10, 64)
		q1, _ := strconv.ParseInt(sq1, 10, 64)
		p2, _ := strconv.ParseInt(sp2, 10, 64)
		q2, _ := strconv.ParseInt(sq2, 10, 64)
		rp, rq := divide(p1, q1, p2, q2)
		fmt.Printf("(%d, %d)\n", rp, rq)
	}
}
