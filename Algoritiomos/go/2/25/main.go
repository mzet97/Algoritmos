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

func AvaliaPolinomio(P *No, x int) int64 {
	var result int64
	var power int64 = 1
	for p := P; p != nil; p = p.Prox {
		result += int64(p.E) * power
		power *= int64(x)
	}
	return result
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
		var x int

		if n != -1 {
			for i := 0; i <= n; i++ {
				e := s.NextInt()
				insereEnc(&P, e, &u)
			}
			x = s.NextInt()
		} else {
			n = 10000000
			for i := 0; i <= n; i++ {
				insereEnc(&P, 1, &u)
			}
			x = 1
		}
		fmt.Printf("%d\n", AvaliaPolinomio(P, x))
		destroi(P)
	}
}
