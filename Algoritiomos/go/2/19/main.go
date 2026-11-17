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

func Concatenar(L1 *No, L2 *No) *No {
	if L1 == nil {
		return L2
	}
	if L2 == nil {
		return L1
	}
	p := L1
	for p.Prox != nil {
		p = p.Prox
	}
	p.Prox = L2
	return L1
}

func insere(L **No, e int, u **No) {
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
	for p := L; p != nil; p = p.Prox {
		fmt.Printf("%d ", p.E)
	}
	fmt.Println("]")
}

func destroi(L *No) {
	p := L
	for p != nil {
		pr := p
		p = p.Prox
		_ = pr
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
		maxN1 := s.NextInt()
		maxN2 := s.NextInt()

		if maxN1 != -1 {
			var L1, L2, u *No

			u = nil
			for i := 0; i < maxN1; i++ {
				e := s.NextInt()
				insere(&L1, e, &u)
			}
			u = nil
			for i := 0; i < maxN2; i++ {
				e := s.NextInt()
				insere(&L2, e, &u)
			}
			L := Concatenar(L1, L2)
			escreve(L)
			destroi(L)
		} else {
			maxN1 = 500000
			maxN2 = 500000
			var L1, L2, u *No

			u = nil
			for i := 0; i < maxN1; i++ {
				insere(&L1, 1, &u)
			}
			u = nil
			for i := 0; i < maxN2; i++ {
				insere(&L2, 2, &u)
			}
			L := Concatenar(L1, L2)
			escreve(L)
			destroi(L)
		}
	}
}
