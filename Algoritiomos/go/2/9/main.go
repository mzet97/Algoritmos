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

func Soma2a2(L *No) *No {
	var vals []int
	for p := L; p != nil; p = p.Prox {
		vals = append(vals, p.E)
	}
	var result []int
	for i := 0; i < len(vals); i++ {
		for j := i + 1; j < len(vals); j++ {
			result = append(result, vals[i]+vals[j])
		}
	}
	sort.Ints(result)
	var head, tail *No
	for _, v := range result {
		novo := &No{E: v}
		if head == nil {
			head = novo
		} else {
			tail.Prox = novo
		}
		tail = novo
	}
	return head
}

func escreve(L *No) {
	var vals []int
	for p := L; p != nil; p = p.Prox {
		vals = append(vals, p.E)
	}
	sort.Ints(vals)
	fmt.Print("[ ")
	for _, v := range vals {
		fmt.Printf("%d ", v)
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
			Lr := Soma2a2(L)
			escreve(Lr)
			destroi(&Lr)
			destroi(&L)
		} else {
			L = nil
			n = 1000
			for i := 0; i < n; i++ {
				insere(&L, i)
			}
			Lr := Soma2a2(L)
			escreve(Lr)
			destroi(&Lr)
			destroi(&L)
		}
	}
}
