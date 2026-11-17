package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

func MaiorDiferenca(L []int, n int) int {
	if n < 2 {
		return 0
	}
	minVal := L[0]
	maxDiff := L[1] - L[0]
	for i := 1; i < n; i++ {
		diff := L[i] - minVal
		if diff > maxDiff {
			maxDiff = diff
		}
		if L[i] < minVal {
			minVal = L[i]
		}
	}
	return maxDiff
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
		if maxN != -1 {
			L = make([]int, maxN)
			for i := 0; i < maxN; i++ {
				scanner.Scan()
				L[i], _ = strconv.Atoi(strings.TrimSpace(scanner.Text()))
			}
			n = maxN
		} else {
			maxN = 1000000
			L = make([]int, maxN)
			n = maxN
			for i := 0; i < maxN; i++ {
				L[i] = maxN - i
			}
			L[500000] = -1
			L[70000] = 3000000
		}
		for t := 0; t < 200; t++ {
			MaiorDiferenca(L, n)
		}
		fmt.Printf("%d\n", MaiorDiferenca(L, n))
	}
}
