package main

import (
	"bufio"
	"fmt"
	"os"
	"sort"
	"strconv"
	"strings"
)

func SomaMaisProximaZero(L []int, n int) int {
	sort.Ints(L[:n])
	lo := 0
	hi := n - 1
	best := L[lo] + L[hi]
	for lo < hi {
		sum := L[lo] + L[hi]
		if abs(sum) < abs(best) {
			best = sum
		}
		if sum == 0 {
			return 0
		}
		if sum < 0 {
			lo++
		} else {
			hi--
		}
	}
	return best
}

func abs(x int) int {
	if x < 0 {
		return -x
	}
	return x
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
			maxN = 100000
			L = make([]int, maxN)
			n = maxN
			for i := 1; i <= maxN; i++ {
				L[i-1] = i
			}
			L[50000] = -20000
		}
		fmt.Printf("%d\n", SomaMaisProximaZero(L, n))
	}
}
