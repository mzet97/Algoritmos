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

func SomaPolinomios(P *No, Q *No) *No {
	var head, tail *No
	p := P
	q := Q
	for p != nil && q != nil {
		novo := &No{E: p.E + q.E, Prox: nil}
		if head == nil {
			head = novo
		} else {
			tail.Prox = novo
		}
		tail = novo
		p = p.Prox
		q = q.Prox
	}
	for p != nil {
		novo := &No{E: p.E, Prox: nil}
		if head == nil {
			head = novo
		} else {
			tail.Prox = novo
		}
		tail = novo
		p = p.Prox
	}
	for q != nil {
		novo := &No{E: q.E, Prox: nil}
		if head == nil {
			head = novo
		} else {
			tail.Prox = novo
		}
		tail = novo
		q = q.Prox
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
		n1 := s.NextInt()
		n2 := s.NextInt()

		var P, Q, u *No

		if n1 != -1 {
			u = nil
			for i := 0; i <= n1; i++ {
				e := s.NextInt()
				insereEnc(&P, e, &u)
			}
			u = nil
			for i := 0; i <= n2; i++ {
				e := s.NextInt()
				insereEnc(&Q, e, &u)
			}
		} else {
			n1 = 1000000
			n2 = 1000000
			u = nil
			insereEnc(&P, 0, &u)
			for i := 0; i < n1; i++ {
				insereEnc(&P, 1, &u)
			}
			u = nil
			insereEnc(&Q, 1, &u)
			for i := 0; i < n2; i++ {
				insereEnc(&Q, -1, &u)
			}
		}
		R := SomaPolinomios(P, Q)
		escreve(R)
		destroi(R)
		destroi(P)
		destroi(Q)
	}
}
