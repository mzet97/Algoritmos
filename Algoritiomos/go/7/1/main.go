package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

func Histograma(X []int, n, k int) []int {
	H := make([]int, k)
	for i := 0; i < n; i++ {
		if X[i] >= 0 && X[i] < k {
			H[X[i]]++
		}
	}
	return H
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
		var L []int
		n := 0
		var k int
		if maxN != -1 {
			L = make([]int, maxN)
			for i := 0; i < maxN; i++ {
				scanner.Scan()
				L[i], _ = strconv.Atoi(strings.TrimSpace(scanner.Text()))
			}
			n = maxN
			scanner.Scan()
			k, _ = strconv.Atoi(strings.TrimSpace(scanner.Text()))
		} else {
			maxN = 1000000
			L = make([]int, maxN)
			n = maxN
			for i := 0; i < maxN; i++ {
				L[i] = maxN - i
			}
			k = 100000
		}
		H := Histograma(L, n, k)
		for i := 0; i < k; i++ {
			fmt.Printf("%d: %d; ", i, H[i])
		}
		fmt.Println()
	}
}
