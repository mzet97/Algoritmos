package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
)

func Subtrai(A []int, n int, B []int, m int) []int {
	R := make([]int, n)
	emprestimo := 0
	j := m - 1
	for i := n - 1; i >= 0; i-- {
		diff := A[i] - emprestimo
		if j >= 0 {
			diff -= B[j]
			j--
		}
		if diff < 0 {
			diff += 10
			emprestimo = 1
		} else {
			emprestimo = 0
		}
		R[i] = diff
	}
	// Remove leading zeros
	k := 0
	for k < len(R)-1 && R[k] == 0 {
		k++
	}
	return R[k:]
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for scanner.Scan() {
		line := scanner.Text()
		if line == "" {
			continue
		}
		var n, m int
		_, err := fmt.Sscanf(line, "%d %d", &n, &m)
		if err != nil {
			continue
		}
		A := make([]int, n)
		for i := 0; i < n; i++ {
			scanner.Scan()
			A[i], _ = strconv.Atoi(scanner.Text())
		}
		B := make([]int, m)
		for i := 0; i < m; i++ {
			scanner.Scan()
			B[i], _ = strconv.Atoi(scanner.Text())
		}
		R := Subtrai(A, n, B, m)
		for i, d := range R {
			if i > 0 {
				fmt.Printf(" ")
			}
			fmt.Printf("%d", d)
		}
		fmt.Println()
	}
}
