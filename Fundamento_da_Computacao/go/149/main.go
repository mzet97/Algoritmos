package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func polinomio(N int, a []int, k int) int {
	// Horner's method: f(k) = a[N]*k^N + ... + a[1]*k + a[0]
	result := a[N]
	for i := N - 1; i >= 0; i-- {
		result = result*k + a[i]
	}
	return result
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sn, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		N, _ := strconv.Atoi(sn)
		var a []int
		if N >= 0 {
			a = make([]int, N+1)
			for i := 0; i <= N; i++ {
				s, err := bib1.Entrada(scanner)
				if err != nil {
					return
				}
				a[i], _ = strconv.Atoi(s)
			}
			sk, err := bib1.Entrada(scanner)
			if err != nil {
				return
			}
			k, _ := strconv.Atoi(sk)
			fmt.Println(polinomio(N, a, k))
		} else {
			N = 100000
			a = make([]int, N+1)
			for i := 0; i <= N; i++ {
				a[i] = 1
			}
			k := 1
			fmt.Println(polinomio(N, a, k))
		}
	}
}
