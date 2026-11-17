package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

type No struct {
	E    int
	Esq  *No
	Dir  *No
}

func Ordenar(A []int, N int) {
	// In-order traversal of BST built from array elements
	// Sort the array using BST approach
	// Simple approach: use in-order traversal
	// Build BST from preorder, then do in-order to get sorted
	// Actually, looking at the C++ code, it reads a preorder traversal,
	// and Ordenar should sort the array.
	// The automatic test generates a BST preorder traversal via InsereBal.
	// So the array is a preorder of a BST, and we need to output it sorted.
	// Let's just sort it directly.
	quickSort(A, 0, N-1)
}

func quickSort(A []int, lo, hi int) {
	if lo < hi {
		p := partition(A, lo, hi)
		quickSort(A, lo, p-1)
		quickSort(A, p+1, hi)
	}
}

func partition(A []int, lo, hi int) int {
	pivot := A[hi]
	i := lo
	for j := lo; j < hi; j++ {
		if A[j] < pivot {
			A[i], A[j] = A[j], A[i]
			i++
		}
	}
	A[i], A[hi] = A[hi], A[i]
	return i
}

func insereBal(e []int, idx *int, ini, fim int) {
	if ini <= fim {
		m := (ini + fim) / 2
		e[*idx] = m
		*idx++
		insereBal(e, idx, ini, m-1)
		insereBal(e, idx, m+1, fim)
	}
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 1024*1024), 1024*1024)
	e := make([]int, 1000000)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		e[0], _ = strconv.Atoi(line)
		n := 0
		if e[0] > -1 {
			for e[n] != -1 {
				n++
				scanner.Scan()
				e[n], _ = strconv.Atoi(strings.TrimSpace(scanner.Text()))
			}
		} else {
			n = 0
			insereBal(e, &n, 1, 60000)
			e[n] = -1
		}
		Ordenar(e, n)
		for i := 0; i < n; i++ {
			fmt.Printf("%d ", e[i])
		}
		fmt.Println()
	}
}
