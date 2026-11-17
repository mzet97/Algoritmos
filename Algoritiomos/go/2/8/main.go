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

func insere(L **No, e int) {
	novo := &No{E: e, Prox: *L}
	*L = novo
}

func SemRepeticoes(L *No) *No {
	var result *No
	var tail *No
	seen := make(map[int]bool)
	for p := L; p != nil; p = p.Prox {
		if !seen[p.E] {
			seen[p.E] = true
			novo := &No{E: p.E}
			if result == nil {
				result = novo
			} else {
				tail.Prox = novo
			}
			tail = novo
		}
	}
	return result
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
		n := s.NextInt()
		if n > -1 {
			L = nil
			for i := 0; i < n; i++ {
				e := s.NextInt()
				insere(&L, e)
			}
			Lr := SemRepeticoes(L)
			escreve(Lr)
			destroi(&Lr)
			destroi(&L)
		} else {
			nlin := n
			n = 1000
			L = nil
			for i := 0; i < n; i++ {
				if i%2 == 0 {
					insere(&L, 2)
				} else {
					insere(&L, 3)
				}
			}
			if nlin != -1 {
				for i := 0; i < n; i++ {
					insere(&L, i)
				}
			}
			Lr := SemRepeticoes(L)
			escreve(Lr)
			destroi(&Lr)
			destroi(&L)
		}
	}
}
