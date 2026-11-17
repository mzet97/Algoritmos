package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

func Maximo(A []int, n int) int {
	m := A[0]
	for i := 1; i < n; i++ {
		if A[i] > m {
			m = A[i]
		}
	}
	return m
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 1024*1024), 1024*1024)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		n, _ := strconv.Atoi(line)
		if n >= 0 {
			A := make([]int, n)
			for i := 0; i < n; i++ {
				scanner.Scan()
				A[i], _ = strconv.Atoi(strings.TrimSpace(scanner.Text()))
			}
			fmt.Printf("%d\n", Maximo(A, n))
		} else {
			n = 50000
			A := make([]int, n)
			for i := 0; i < n/2; i++ {
				A[i] = i
			}
			for i := n/2 + 1; i < n; i++ {
				A[i] = n - i
			}
			for i := 0; i < 20; i++ {
				Maximo(A, n)
			}
			fmt.Printf("%d\n", Maximo(A, n))
		}
	}
}
