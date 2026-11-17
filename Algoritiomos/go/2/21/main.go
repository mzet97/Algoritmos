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

type ListaLinear struct {
	Inicio *No
	N      int
}

func Diferenca(L1 ListaLinear, L2 ListaLinear) ListaLinear {
	set := make(map[int]bool)
	for p := L2.Inicio; p != nil; p = p.Prox {
		set[p.E] = true
	}
	var result ListaLinear
	var u *No
	for p := L1.Inicio; p != nil; p = p.Prox {
		if !set[p.E] {
			novo := &No{E: p.E, Prox: nil}
			if result.Inicio == nil {
				result.Inicio = novo
			} else {
				u.Prox = novo
			}
			u = novo
			result.N++
		}
	}
	return result
}

func insere(L *ListaLinear, e int, u **No) {
	novo := &No{E: e, Prox: nil}
	if *u == nil {
		L.Inicio = novo
	} else {
		(*u).Prox = novo
	}
	*u = novo
	L.N++
}

func escreve(L ListaLinear) {
	vals := make([]int, L.N)
	p := L.Inicio
	for i := 0; i < L.N; i++ {
		vals[i] = p.E
		p = p.Prox
	}
	sort.Ints(vals)
	fmt.Print("[")
	for _, v := range vals {
		fmt.Printf("%d ", v)
	}
	fmt.Println("]")
}

func destroi(L ListaLinear) {
	p := L.Inicio
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
			var L1, L2 ListaLinear
			var u *No

			L1.Inicio = nil
			L1.N = 0
			u = nil
			for i := 0; i < maxN1; i++ {
				e := s.NextInt()
				insere(&L1, e, &u)
			}
			L2.Inicio = nil
			L2.N = 0
			u = nil
			for i := 0; i < maxN2; i++ {
				e := s.NextInt()
				insere(&L2, e, &u)
			}
			L := Diferenca(L1, L2)
			escreve(L)
			destroi(L1)
			destroi(L2)
			destroi(L)
		} else {
			maxN1 = 9000
			maxN2 = 9000
			var L1, L2 ListaLinear
			var u *No

			L1.Inicio = nil
			L1.N = 0
			u = nil
			for i := 0; i < maxN1; i++ {
				insere(&L1, i*2, &u)
			}
			L2.Inicio = nil
			L2.N = 0
			u = nil
			for i := 0; i < maxN2; i++ {
				insere(&L2, i*3, &u)
			}
			L := Diferenca(L1, L2)
			escreve(L)
			destroi(L1)
			destroi(L2)
			destroi(L)
		}
	}
}
