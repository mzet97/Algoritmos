package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

func SaoAnagramas(A, B string, n int) int {
	count := make([]int, 256)
	for i := 0; i < n; i++ {
		count[A[i]]++
		count[B[i]]--
	}
	for i := 0; i < 256; i++ {
		if count[i] != 0 {
			return 0
		}
	}
	return 1
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 1024*1024), 1024*1024)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		maxN, _ := strconv.Atoi(line)
		A, B := "", ""
		n := maxN
		if maxN != -1 {
			scanner.Scan()
			A = strings.TrimSpace(scanner.Text())
			scanner.Scan()
			B = strings.TrimSpace(scanner.Text())
		} else {
			maxN = 1000000
			aBytes := make([]byte, maxN)
			bBytes := make([]byte, maxN)
			alphabet := int('z'-'a') + 1
			for i := maxN - 1; i >= 0; i-- {
				aBytes[i] = byte('a' + (i % alphabet))
			}
			for i := 0; i < maxN; i++ {
				bBytes[i] = byte('a' + (i % alphabet))
			}
			A = string(aBytes)
			B = string(bBytes)
			n = maxN
		}
		fmt.Printf("%d\n", SaoAnagramas(A, B, n))
	}
}
