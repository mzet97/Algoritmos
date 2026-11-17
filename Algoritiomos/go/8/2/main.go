package main

import (
	"bufio"
	"container/heap"
	"fmt"
	"os"
	"strconv"
	"strings"
)

type MaxHeap []int

func (h MaxHeap) Len() int            { return len(h) }
func (h MaxHeap) Less(i, j int) bool  { return h[i] > h[j] }
func (h MaxHeap) Swap(i, j int)       { h[i], h[j] = h[j], h[i] }
func (h *MaxHeap) Push(x interface{}) { *h = append(*h, x.(int)) }
func (h *MaxHeap) Pop() interface{} {
	old := *h
	n := len(old)
	x := old[n-1]
	*h = old[:n-1]
	return x
}

func Ordenar(L []int, n int) {
	h := &MaxHeap{}
	heap.Init(h)
	for i := 0; i < n; i++ {
		heap.Push(h, L[i])
	}
	for i := n - 1; i >= 0; i-- {
		L[i] = heap.Pop(h).(int)
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
		if n != -1 {
			L = make([]int, n)
			for i := 0; i < n; i++ {
				scanner.Scan()
				L[i], _ = strconv.Atoi(strings.TrimSpace(scanner.Text()))
			}
		} else {
			n = 100000
			L = make([]int, n)
			for i := 0; i < n; i++ {
				L[i] = n - i
			}
		}
		Ordenar(L, n)
		for i := 0; i < n; i++ {
			fmt.Printf("%d ", L[i])
		}
		fmt.Println()
	}
}
