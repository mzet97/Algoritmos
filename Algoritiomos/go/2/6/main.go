package main

import (
	"bufio"
	"fmt"
	"os"
	"sort"
	"strconv"
	"strings"
)

func RemoveMaiores(L []int, n *int, k int) {
	j := 0
	for i := 0; i < *n; i++ {
		if L[i] <= k {
			L[j] = L[i]
			j++
		}
	}
	*n = j
}

func escreve(L []int, n int) {
	v := make([]int, n)
	copy(v, L[:n])
	sort.Ints(v)
	fmt.Print("[")
	for i := 0; i < n; i++ {
		fmt.Print(v[i])
	}
	fmt.Println("]")
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 1024*1024), 1024*1024)

	// Collect all integers from input
	var nums []int
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		fields := strings.Fields(line)
		for _, f := range fields {
			v, err := strconv.Atoi(f)
			if err != nil {
				continue
			}
			nums = append(nums, v)
		}
	}

	idx := 0
	for idx < len(nums) {
		maxN := nums[idx]
		idx++
		if maxN != -1 {
			L := make([]int, maxN)
			n := 0
			for i := 0; i < maxN && idx < len(nums); i++ {
				L[n] = nums[idx]
				n++
				idx++
			}
			if idx >= len(nums) {
				break
			}
			k := nums[idx]
			idx++
			RemoveMaiores(L, &n, k)
			escreve(L, n)
		} else {
			maxN = 1000000
			L := make([]int, maxN)
			n := 0
			for i := 0; i < maxN; i++ {
				if i%2 == 0 {
					L[n] = 2
				} else {
					L[n] = 3
				}
				n++
			}
			RemoveMaiores(L, &n, 2)
			escreve(L, n)
		}
	}
}
