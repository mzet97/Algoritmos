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

func ProdutoPolinomioEscalar(P *No, a int) *No {
	var head, tail *No
	for p := P; p != nil; p = p.Prox {
		novo := &No{E: p.E * a, Prox: nil}
		if head == nil {
			head = novo
		} else {
			tail.Prox = novo
		}
		tail = novo
	}
	return head
}

func insereEnc(L **No, e int, u **No) {
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
		n := s.NextInt()
		var P, u *No
		var a int

		if n != -1 {
			u = nil
			for i := 0; i <= n; i++ {
				e := s.NextInt()
				insereEnc(&P, e, &u)
			}
			a = s.NextInt()
		} else {
			n = 500000
			u = nil
			for i := 0; i <= n; i++ {
				insereEnc(&P, 1, &u)
			}
			a = 2
		}
		R := ProdutoPolinomioEscalar(P, a)
		escreve(R)
		destroi(R)
		destroi(P)
	}
}
