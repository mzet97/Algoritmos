package main

import (
	"bufio"
	"fmt"
	"os"
	"sort"
	"strconv"
	"strings"
)

func ProcuraSoma(L []int, n, k int) int {
	sort.Ints(L[:n])
	lo := 0
	hi := n - 1
	for lo < hi {
		sum := L[lo] + L[hi]
		if sum == k {
			return 1
		}
		if sum < k {
			lo++
		} else {
			hi--
		}
	}
	return 0
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
		var k int
		n := 0
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
				L[i] = i
			}
			L[200000] = 2000000
			k = 2600000
		}
		fmt.Printf("%d\n", ProcuraSoma(L, n, k))
	}
}
