package main

import (
	"bufio"
	"fmt"
	"os"

)

func ConcatenaPrefixos(A string, B string, n int, m int) string {
	result := make([]byte, n+m)
	for i := 0; i < n; i++ {
		result[i] = A[i]
	}
	for i := 0; i < m; i++ {
		result[n+i] = B[i]
	}
	return string(result)
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
		scanner.Scan()
		A := scanner.Text()
		scanner.Scan()
		B := scanner.Text()
		fmt.Printf("%s\n", ConcatenaPrefixos(A, B, n, m))
	}
}
