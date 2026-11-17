package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

func Distintos(L []int, N int) int {
	seen := make(map[int]bool)
	for i := 0; i < N; i++ {
		seen[L[i]] = true
	}
	return len(seen)
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
				L[i] = i % 997
			}
		}
		fmt.Printf("%d\n", Distintos(L, n))
	}
}
