package main

import (
	"bufio"
	"fmt"
	"os"
	"sort"
	"strconv"
	"strings"
)

type No struct {
	E    int
	Prox *No
}

func QuantosContidos(LS []int, N int, LE *No) int {
	sort.Ints(LS[:N])
	count := 0
	for p := LE; p != nil; p = p.Prox {
		idx := sort.SearchInts(LS[:N], p.E)
		if idx < N && LS[idx] == p.E {
			count++
		}
	}
	return count
}

func insereEnc(LE **No, x int) {
	novo := &No{E: x, Prox: *LE}
	*LE = novo
}

func desalocarEnc(LE *No) {
	p := LE
	for p != nil {
		nxt := p.Prox
		_ = nxt
		p = nxt
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
		n := s.NextInt()
		if n > -1 {
			LS := make([]int, n)
			N := 0
			for i := 0; i < n; i++ {
				LS[N] = s.NextInt()
				N++
			}
			var LE *No
			for i := 0; i < n*n; i++ {
				x := s.NextInt()
				insereEnc(&LE, x)
			}
			fmt.Printf("%d\n", QuantosContidos(LS, N, LE))
			desalocarEnc(LE)
		} else {
			n = 3000
			LS := make([]int, n)
			N := 0
			for i := 0; i < n; i++ {
				LS[N] = 1
				N++
			}
			var LE *No
			for i := 0; i < n*n; i++ {
				insereEnc(&LE, 2)
			}
			fmt.Printf("%d\n", QuantosContidos(LS, N, LE))
			desalocarEnc(LE)
		}
	}
}
