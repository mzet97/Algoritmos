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

func Remove(L **No, p *No) {
	if *L == p {
		*L = p.Prox
		return
	}
	prev := *L
	for prev.Prox != p {
		prev = prev.Prox
	}
	prev.Prox = p.Prox
}

func insere(L **No, e int) *No {
	novo := &No{E: e, Prox: *L}
	*L = novo
	return novo
}

func escreve(L *No) {
	fmt.Print("[ ")
	for p := L; p != nil; p = p.Prox {
		fmt.Printf("%d ", p.E)
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
		n := s.NextInt()
		if n != -1 {
			L = nil
			V := make([]*No, n)
			for i := 0; i < n; i++ {
				e := s.NextInt()
				V[i] = insere(&L, e)
			}
			idx := s.NextInt()
			Remove(&L, V[idx-1])
			escreve(L)
			destroi(&L)
		} else {
			L = nil
			n = 100000
			V := make([]*No, n)
			for i := 0; i < n; i++ {
				V[i] = insere(&L, i%100)
			}
			for i := 0; i < 10; i++ {
				Remove(&L, V[i*9000])
			}
			escreve(L)
			destroi(&L)
		}
	}
}
