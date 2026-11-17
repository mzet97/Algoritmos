package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
)

func Maior(A []int, n int) int {
	maior := A[0]
	for i := 1; i < n; i++ {
		if A[i] > maior {
			maior = A[i]
		}
	}
	return maior
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for scanner.Scan() {
		line := scanner.Text()
		if line == "" {
			continue
		}
		n, err := strconv.Atoi(line)
		if err != nil {
			continue
		}
		A := make([]int, n)
		for i := 0; i < n; i++ {
			scanner.Scan()
			A[i], _ = strconv.Atoi(scanner.Text())
		}
		fmt.Printf("%d\n", Maior(A, n))
	}
}
