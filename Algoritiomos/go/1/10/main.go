package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
)

func Soma(A []int, n int, B []int, m int) []int {
	R := make([]int, 0)
	carry := 0
	i := n - 1
	j := m - 1
	for i >= 0 || j >= 0 || carry > 0 {
		sum := carry
		if i >= 0 {
			sum += A[i]
			i--
		}
		if j >= 0 {
			sum += B[j]
			j--
		}
		R = append([]int{sum % 10}, R...)
		carry = sum / 10
	}
	return R
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
		R := Soma(A, n, B, m)
		for i, d := range R {
			if i > 0 {
				fmt.Printf(" ")
			}
			fmt.Printf("%d", d)
		}
		fmt.Println()
	}
}
