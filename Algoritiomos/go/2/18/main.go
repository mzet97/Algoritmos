package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

func Concatenar(L1 []int, n1 int, L2 []int, n2 int) []int {
	L := make([]int, n1+n2)
	copy(L[:n1], L1[:n1])
	copy(L[n1:], L2[:n2])
	return L
}

type IntScanner struct {
	scanner *bufio.Scanner
	tokens  []string
	pos     int
}

func NewIntScanner() *IntScanner {
	s := &IntScanner{scanner: bufio.NewScanner(os.Stdin)}
	s.scanner.Buffer(make([]byte, 1024*1024), 1024*1024)
	return s
}

func (s *IntScanner) NextInt() int {
	for s.pos >= len(s.tokens) {
		if !s.scanner.Scan() {
			return 0
		}
		s.tokens = strings.Fields(s.scanner.Text())
		s.pos = 0
	}
	v, _ := strconv.Atoi(s.tokens[s.pos])
	s.pos++
	return v
}

func main() {
	s := NewIntScanner()

	for {
		maxN1 := s.NextInt()
		maxN2 := s.NextInt()
		if maxN1 != -1 {
			L1 := make([]int, maxN1)
			n1 := 0
			for i := 0; i < maxN1; i++ {
				L1[n1] = s.NextInt()
				n1++
			}
			L2 := make([]int, maxN2)
			n2 := 0
			for i := 0; i < maxN2; i++ {
				L2[n2] = s.NextInt()
				n2++
			}
			L := Concatenar(L1, n1, L2, n2)
			fmt.Print("[")
			for i := 0; i < n1+n2; i++ {
				fmt.Printf("%d ", L[i])
			}
			fmt.Println("]")
		} else {
			maxN1 = 500000
			maxN2 = 500000
			L1 := make([]int, maxN1)
			n1 := 0
			L2 := make([]int, maxN2)
			n2 := 0
			for i := 0; i < maxN1; i++ {
				L1[n1] = 1
				n1++
				L2[n2] = 2
				n2++
			}
			L := Concatenar(L1, n1, L2, n2)
			fmt.Print("[")
			for i := 0; i < n1+n2; i++ {
				fmt.Printf("%d ", L[i])
			}
			fmt.Println("]")
		}
	}
}
