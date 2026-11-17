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

func insereInicio(L **No, e int) {
	novo := &No{E: e, Prox: *L}
	*L = novo
}

func ProdutoPolinomios(P *No, Q *No) *No {
	// R starts as the zero polynomial [0]
	R := &No{E: 0, Prox: nil}

	zeros := 0
	p := P
	for p != nil {
		T := ProdutoPolinomioEscalar(Q, p.E)
		for i := 1; i <= zeros; i++ {
			insereInicio(&T, 0)
		}
		Rtemp := SomaPolinomios(R, T)
		R = Rtemp
		zeros++
		p = p.Prox
	}
	return R
}

func insereFinalEnc(L **No, e int, u **No) {
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
				insereFinalEnc(&P, e, &u)
			}
			u = nil
			for i := 0; i <= n2; i++ {
				e := s.NextInt()
				insereFinalEnc(&Q, e, &u)
			}
		} else {
			n1 = 1000
			n2 = 1000
			u = nil
			for i := 0; i <= n1; i++ {
				insereFinalEnc(&P, 1, &u)
			}
			u = nil
			for i := 0; i <= n2; i++ {
				insereFinalEnc(&Q, 1, &u)
			}
		}
		R := ProdutoPolinomios(P, Q)
		escreve(R)
		destroi(R)
		destroi(P)
		destroi(Q)
	}
}
