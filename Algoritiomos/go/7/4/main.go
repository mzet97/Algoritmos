package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

func NumeroMaisFrequente(L []int64, n int) int64 {
	if n == 0 {
		return 0
	}
	freq := make(map[int64]int)
	bestVal := L[0]
	bestFreq := 0
	for i := 0; i < n; i++ {
		freq[L[i]]++
		if freq[L[i]] > bestFreq || (freq[L[i]] == bestFreq && L[i] < bestVal) {
			bestFreq = freq[L[i]]
			bestVal = L[i]
		}
	}
	return bestVal
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
		var L []int64
		n := 0
		if maxN != -1 {
			L = make([]int64, maxN)
			n = maxN
			for i := 0; i < maxN; i++ {
				scanner.Scan()
				L[i], _ = strconv.ParseInt(strings.TrimSpace(scanner.Text()), 10, 64)
			}
		} else {
			maxN = 3000000
			L = make([]int64, maxN)
			n = maxN
			for i := 0; i < maxN; i++ {
				L[i] = int64(maxN-i) * 1000000
			}
			L[100000] = 1000000 * 191191
		}
		fmt.Printf("%d\n", NumeroMaisFrequente(L, n))
	}
}
