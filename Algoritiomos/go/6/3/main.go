package main

import (
	"bufio"
	"fmt"
	"os"
	"sort"
	"strconv"
	"strings"
)

func QuantidadeIntervalo(L []int, n, a, b int) int {
	lo := sort.SearchInts(L[:n], a)
	hi := sort.SearchInts(L[:n], b+1)
	return hi - lo
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
		var a, b int
		n := 0
		if maxN != -1 {
			L = make([]int, maxN)
			for i := 0; i < maxN; i++ {
				scanner.Scan()
				L[i], _ = strconv.Atoi(strings.TrimSpace(scanner.Text()))
			}
			n = maxN
			scanner.Scan()
			parts := strings.Fields(strings.TrimSpace(scanner.Text()))
			a, _ = strconv.Atoi(parts[0])
			b, _ = strconv.Atoi(parts[1])
		} else {
			maxN = 1000000
			L = make([]int, maxN)
			n = maxN
			for i := 0; i < maxN; i++ {
				L[i] = 3 * i
			}
			a = 3 * 200000
			b = 3 * 900000
		}
		sort.Ints(L[:n])
		for t := 0; t < 100000; t++ {
			QuantidadeIntervalo(L, n, a, b)
		}
		fmt.Printf("%d\n", QuantidadeIntervalo(L, n, a, b))
	}
}
