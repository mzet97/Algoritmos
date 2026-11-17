package main

import (
	"bufio"
	"fmt"
	"os"
)

func PermutacaoCircular(A []byte, n int, k int) {
	if n == 0 {
		return
	}
	k = k % n
	if k == 0 {
		return
	}
	// Reverse entire array, Reverse first k. Reverse remaining.
	reverse(A, 0, n-1)
	reverse(A, 0, k-1)
	reverse(A, k, n-1)
}

func reverse(A []byte, l int, r int) {
	for l < r {
		A[l], A[r] = A[r], A[l]
		l++
		r--
	}
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 1024*1024), 1024*1024)
	for scanner.Scan() {
		line := scanner.Text()
		if line == "" {
			continue
		}
		var n, k int
		_, err := fmt.Sscanf(line, "%d %d", &n, &k)
		if err != nil {
			continue
		}
		scanner.Scan()
		A := []byte(scanner.Text())
		PermutacaoCircular(A, n, k)
		fmt.Printf("%s\n", string(A))
	}
}
