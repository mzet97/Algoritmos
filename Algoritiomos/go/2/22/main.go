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
	Prox *No
}

func InsereBuscaRemove(L *No, x int) bool {
	// (i) Insert x in sorted order (L is head, last is tail)
	prev := L
	for prev.Prox != nil && prev.Prox.Prox != nil && prev.Prox.E < x {
		prev = prev.Prox
	}
	novo := &No{E: x, Prox: prev.Prox}
	prev.Prox = novo

	// (ii) Check if 2*x is in the list
	found := false
	for p := L.Prox; p != nil && p.Prox != nil; p = p.Prox {
		if p.E == 2*x {
			found = true
			break
		}
	}

	// (iii) Remove all elements equal to 3*x
	prev2 := L
	for prev2.Prox != nil && prev2.Prox.Prox != nil {
		if prev2.Prox.E == 3*x {
			prev2.Prox = prev2.Prox.Prox
		} else {
			prev2 = prev2.Prox
		}
	}

	return found
}

func insereFinal(L **No, e int, u **No) {
	novo := &No{E: e, Prox: nil}
	if *u == nil {
		*L = novo
	} else {
		(*u).Prox = novo
	}
	*u = novo
}

func escreve(L *No) {
	fmt.Print("[")
	p := L.Prox
	for p.Prox != nil {
		fmt.Printf("%d ", p.E)
		p = p.Prox
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

	for {
		n := s.NextInt()
		if n != -1 {
			var L, u *No
			insereFinal(&L, 3000, &u)
			for i := 0; i < n; i++ {
				x := s.NextInt()
				insereFinal(&L, x, &u)
			}
			insereFinal(&L, 3000, &u)
			x := s.NextInt()
			r := InsereBuscaRemove(L, x)
			if r {
				fmt.Print("1 ")
			} else {
				fmt.Print("0 ")
			}
			escreve(L)
			destroi(&L)
		} else {
			n = 500000
			var L, u *No
			insereFinal(&L, 3000, &u)
			for i := 0; i < n; i++ {
				insereFinal(&L, i, &u)
			}
			insereFinal(&L, 3000, &u)
			r := InsereBuscaRemove(L, 300000)
			if r {
				fmt.Print("1 ")
			} else {
				fmt.Print("0 ")
			}
			escreve(L)
			destroi(&L)
		}
	}
}
