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

func RemoveMaiores(L **No, k int) {
	for *L != nil && (*L).E > k {
		*L = (*L).Prox
	}
	if *L == nil {
		return
	}
	p := *L
	for p.Prox != nil {
		if p.Prox.E > k {
			p.Prox = p.Prox.Prox
		} else {
			p = p.Prox
		}
	}
}

func insere(L **No, e int) {
	novo := &No{E: e, Prox: *L}
	*L = novo
}

func escreve(L *No) {
	var vals []int
	for p := L; p != nil; p = p.Prox {
		vals = append(vals, p.E)
	}
	sort.Ints(vals)
	fmt.Print("[")
	for _, v := range vals {
		fmt.Print(v)
	}
	fmt.Println("]")
}

func destroi(L **No) {
	p := *L
	for p != nil {
		pr := p
		p = p.Prox
		_ = pr
	}
	*L = nil
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
	var L *No

	for {
		maxN := s.NextInt()
		if maxN != -1 {
			L = nil
			for i := 0; i < maxN; i++ {
				e := s.NextInt()
				insere(&L, e)
			}
			k := s.NextInt()
			RemoveMaiores(&L, k)
			escreve(L)
			destroi(&L)
		} else {
			L = nil
			maxN = 1000000
			for i := 0; i < maxN; i++ {
				if i%2 == 0 {
					insere(&L, 2)
				} else {
					insere(&L, 3)
				}
			}
			RemoveMaiores(&L, 2)
			escreve(L)
			destroi(&L)
		}
	}
}
