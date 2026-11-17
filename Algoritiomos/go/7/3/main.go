package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

func OrdenacaoPorContagem(L []int, n int) {
	if n == 0 {
		return
	}
	maxVal := L[0]
	for i := 1; i < n; i++ {
		if L[i] > maxVal {
			maxVal = L[i]
		}
	}
	count := make([]int, maxVal+1)
	for i := 0; i < n; i++ {
		count[L[i]]++
	}
	idx := 0
	for i := 0; i <= maxVal; i++ {
		for j := 0; j < count[i]; j++ {
			L[idx] = i
			idx++
		}
	}
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
			maxN = 500000
			L = make([]int, maxN)
			for i := maxN; i > 0; i -= 2 {
				L[n] = i
				n++
				L[n] = i
				n++
			}
		}
		OrdenacaoPorContagem(L, n)
		for i := 0; i < n; i++ {
			fmt.Printf("%d,", L[i])
		}
		fmt.Println()
	}
}
