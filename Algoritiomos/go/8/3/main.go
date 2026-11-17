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

type Mediana struct {
	med  float64
	n    int
	ante *MaxHeap
	post *MinHeap
}

func ConstroiMediana(C *Mediana) {
	C.ante = &MaxHeap{}
	C.post = &MinHeap{}
	heap.Init(C.ante)
	heap.Init(C.post)
	C.n = 0
	C.med = 0
}

func DestroiMediana(C *Mediana) {
	C.ante = nil
	C.post = nil
}

func InsereMediana(C *Mediana, x int) {
	if C.n == 0 {
		C.med = float64(x)
		heap.Push(C.ante, x)
	} else if float64(x) <= C.med {
		heap.Push(C.ante, x)
	} else {
		heap.Push(C.post, x)
	}
	for C.ante.Len() > C.post.Len()+1 {
		v := heap.Pop(C.ante).(int)
		heap.Push(C.post, v)
	}
	for C.post.Len() > C.ante.Len() {
		v := heap.Pop(C.post).(int)
		heap.Push(C.ante, v)
	}
	C.n++
	if C.ante.Len() == C.post.Len() {
		C.med = (float64((*C.ante)[0]) + float64((*C.post)[0])) / 2.0
	} else {
		C.med = float64((*C.ante)[0])
	}
}

func ObterMediana(C *Mediana) float64 {
	return C.med
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
				L[i] = i
			}
		}
		var C Mediana
		ConstroiMediana(&C)
		for i := 0; i < n; i++ {
			InsereMediana(&C, L[i])
			fmt.Printf("%.1f ", ObterMediana(&C))
		}
		fmt.Println()
		DestroiMediana(&C)
	}
}
