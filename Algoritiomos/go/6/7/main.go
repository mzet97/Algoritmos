package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

func ParesForaOrdem(L []int, n int) int {
	temp := make([]int, n)
	return mergeCount(L, temp, 0, n-1)
}

func mergeCount(L, temp []int, left, right int) int {
	if left >= right {
		return 0
	}
	mid := (left + right) / 2
	count := mergeCount(L, temp, left, mid)
	count += mergeCount(L, temp, mid+1, right)
	count += merge(L, temp, left, mid, right)
	return count
}

func merge(L, temp []int, left, mid, right int) int {
	i := left
	j := mid + 1
	k := left
	count := 0
	for i <= mid && j <= right {
		if L[i] <= L[j] {
			temp[k] = L[i]
			i++
		} else {
			temp[k] = L[j]
			count += mid - i + 1
			j++
		}
		k++
	}
	for i <= mid {
		temp[k] = L[i]
		i++
		k++
	}
	for j <= right {
		temp[k] = L[j]
		j++
		k++
	}
	for idx := left; idx <= right; idx++ {
		L[idx] = temp[idx]
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
			maxN = 50000
			L = make([]int, maxN)
			n = maxN
			for i := 0; i < maxN; i++ {
				L[i] = maxN - i
			}
		}
		fmt.Printf("%d\n", ParesForaOrdem(L, n))
	}
}
