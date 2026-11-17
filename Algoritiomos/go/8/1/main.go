package main

import (
	"bufio"
	"container/heap"
	"fmt"
	"os"
	"strconv"
	"strings"
)

// MinHeap
type MinHeap []int

func (h MinHeap) Len() int            { return len(h) }
func (h MinHeap) Less(i, j int) bool  { return h[i] < h[j] }
func (h MinHeap) Swap(i, j int)       { h[i], h[j] = h[j], h[i] }
func (h *MinHeap) Push(x interface{}) { *h = append(*h, x.(int)) }
func (h *MinHeap) Pop() interface{} {
	old := *h
	n := len(old)
	x := old[n-1]
	*h = old[:n-1]
	return x
}

func Maiores(L []int, n, k int, M []int) {
	h := &MinHeap{}
	heap.Init(h)
	for i := 0; i < n; i++ {
		if h.Len() < k {
			heap.Push(h, L[i])
		} else if L[i] > (*h)[0] {
			heap.Pop(h)
			heap.Push(h, L[i])
		}
	}
	for i := k - 1; i >= 0; i-- {
		M[i] = heap.Pop(h).(int)
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
		n, _ := strconv.Atoi(line)
		var L []int
		var k int
		if n != -1 {
			L = make([]int, n)
			for i := 0; i < n; i++ {
				scanner.Scan()
				L[i], _ = strconv.Atoi(strings.TrimSpace(scanner.Text()))
			}
			scanner.Scan()
			k, _ = strconv.Atoi(strings.TrimSpace(scanner.Text()))
		} else {
			n = 2000000
			L = make([]int, n)
			for i := 0; i < n; i++ {
				L[i] = i
			}
			k = 100
		}
		M := make([]int, k)
		Maiores(L, n, k, M)
		for i := 0; i < k; i++ {
			fmt.Printf("%d ", M[i])
		}
		fmt.Println()
	}
}
