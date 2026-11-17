package main

import (
	"bufio"
	"fmt"
	"os"
	"sort"
	"strconv"
	"strings"
)

func RemoveMaiores(L []int, n int, k int) int {
	w := 0
	for i := 0; i < n; i++ {
		if L[i] <= k {
			L[w] = L[i]
			w++
		}
	}
	return w
}

func escreve(L []int, n int) {
	Lv := make([]int, n)
	copy(Lv, L[:n])
	sort.Ints(Lv)
	fmt.Printf("[")
	for _, v := range Lv {
		fmt.Printf("%d", v)
	}
	fmt.Printf("]\n")
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 1024*1024), 1024*1024)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		n, _ := strconv.Atoi(line)
		if n != -1 {
			L := make([]int, n)
			for i := 0; i < n; i++ {
				scanner.Scan()
				L[i], _ = strconv.Atoi(strings.TrimSpace(scanner.Text()))
			}
			scanner.Scan()
			k, _ := strconv.Atoi(strings.TrimSpace(scanner.Text()))
			n = RemoveMaiores(L, n, k)
			escreve(L, n)
		} else {
			nlin := 50000
			k := 2
			n = nlin
			L := make([]int, n)
			for t := 1; t <= 20; t++ {
				n = nlin
				for i := 0; i < n; i++ {
					if i%2 == 0 {
						L[i] = 2
					} else {
						L[i] = 3
					}
				}
				n = RemoveMaiores(L, n, k)
				if t == 1 {
					escreve(L, n)
				}
			}
		}
	}
}
