package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

func Inverter(L []int, n int) {
	for i := 0; i < n/2; i++ {
		L[i], L[n-1-i] = L[n-1-i], L[i]
	}
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
		maxN := s.NextInt()
		if maxN != -1 {
			L := make([]int, maxN)
			n := 0
			for i := 0; i < maxN; i++ {
				L[n] = s.NextInt()
				n++
			}
			Inverter(L, n)
			fmt.Print("[")
			for i := 0; i < n; i++ {
				fmt.Printf("%d ", L[i])
			}
			fmt.Println("]")
		} else {
			maxN = 1000000
			L := make([]int, maxN)
			n := 0
			for i := 0; i < maxN; i++ {
				L[n] = i % 10
				n++
			}
			Inverter(L, n)
			fmt.Print("[")
			for i := 0; i < n; i++ {
				fmt.Printf("%d ", L[i])
			}
			fmt.Println("]")
		}
	}
}
