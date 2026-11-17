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

func insere(L **No, e int) {
	novo := &No{E: e, Prox: *L}
	*L = novo
}

func Inverte(L *No) *No {
	var result *No
	for p := L; p != nil; p = p.Prox {
		novo := &No{E: p.E}
		novo.Prox = result
		result = novo
	}
	return result
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
			for i := 0; i < n; i++ {
				e := s.NextInt()
				insere(&L, e)
			}
			Lr := Inverte(L)
			escreve(Lr)
			destroi(&Lr)
			destroi(&L)
		} else {
			L = nil
			n = 500000
			for i := 0; i < n; i++ {
				insere(&L, i%100)
			}
			Lr := Inverte(L)
			escreve(Lr)
			destroi(&Lr)
			destroi(&L)
		}
	}
}
