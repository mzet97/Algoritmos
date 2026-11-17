package main

import (
	"bufio"
	"fmt"
	"os"
	"sort"
	"strconv"
	"strings"
)

type ListaLinear struct {
	E []int
	N int
}

func Intersecao(L1 ListaLinear, L2 ListaLinear) ListaLinear {
	set := make(map[int]bool)
	for i := 0; i < L2.N; i++ {
		set[L2.E[i]] = true
	}
	var result ListaLinear
	result.E = make([]int, 0)
	for i := 0; i < L1.N; i++ {
		if set[L1.E[i]] {
			result.E = append(result.E, L1.E[i])
			result.N++
		}
	}
	return result
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
			var L1, L2 ListaLinear
			L1.E = make([]int, maxN1)
			L1.N = 0
			for i := 0; i < maxN1; i++ {
				L1.E[L1.N] = s.NextInt()
				L1.N++
			}
			L2.E = make([]int, maxN2)
			L2.N = 0
			for i := 0; i < maxN2; i++ {
				L2.E[L2.N] = s.NextInt()
				L2.N++
			}
			L := Intersecao(L1, L2)
			sort.Ints(L.E[:L.N])
			fmt.Print("[")
			for i := 0; i < L.N; i++ {
				fmt.Printf("%d ", L.E[i])
			}
			fmt.Println("]")
		} else {
			maxN1 = 9000
			maxN2 = 9000
			var L1, L2 ListaLinear
			L1.E = make([]int, maxN1)
			L1.N = 0
			L2.E = make([]int, maxN2)
			L2.N = 0
			for i := 0; i < maxN1; i++ {
				L1.E[L1.N] = i * 2
				L1.N++
				L2.E[L2.N] = i * 3
				L2.N++
			}
			L := Intersecao(L1, L2)
			sort.Ints(L.E[:L.N])
			fmt.Print("[")
			for i := 0; i < L.N; i++ {
				fmt.Printf("%d ", L.E[i])
			}
			fmt.Println("]")
		}
	}
}
