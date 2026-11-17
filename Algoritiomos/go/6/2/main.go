package main

import (
	"bufio"
	"fmt"
	"os"
	"sort"
	"strconv"
	"strings"
)

func BuscaEmLote(L []int, B []int, n int) int {
	sort.Ints(L[:n])
	count := 0
	for i := 0; i < n; i++ {
		idx := sort.SearchInts(L[:n], B[i])
		if idx < n && L[idx] == B[i] {
			count++
		}
	}
	return count
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
		var L, B []int
		n := 0
		if maxN != -1 {
			L = make([]int, maxN)
			B = make([]int, maxN)
			for i := 0; i < maxN; i++ {
				scanner.Scan()
				L[i], _ = strconv.Atoi(strings.TrimSpace(scanner.Text()))
			}
			for i := 0; i < maxN; i++ {
				scanner.Scan()
				B[i], _ = strconv.Atoi(strings.TrimSpace(scanner.Text()))
			}
			n = maxN
		} else {
			maxN = 1000000
			L = make([]int, maxN)
			B = make([]int, maxN)
			n = maxN
			for i := 0; i < maxN; i++ {
				L[i] = i
				B[i] = 2 * i
			}
		}
		fmt.Printf("%d\n", BuscaEmLote(L, B, n))
	}
}
