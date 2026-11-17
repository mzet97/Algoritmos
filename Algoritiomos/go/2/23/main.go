package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

type NoEnc struct {
	Chave int
	E     int
	Prox  *NoEnc
}

type NoSeq struct {
	Chave int
	E     int
}

func ConverterSeqParaEnc(L []NoSeq, N int) *NoEnc {
	var head, tail *NoEnc
	for i := 0; i < N; i++ {
		novo := &NoEnc{Chave: L[i].Chave, E: L[i].E, Prox: nil}
		if head == nil {
			head = novo
		} else {
			tail.Prox = novo
		}
		tail = novo
	}
	return head
}

func ConverterEncParaSeq(L *NoEnc, N *int) []NoSeq {
	*N = 0
	p := L
	for p != nil {
		(*N)++
		p = p.Prox
	}
	result := make([]NoSeq, *N)
	p = L
	for i := 0; i < *N; i++ {
		result[i].Chave = p.Chave
		result[i].E = p.E
		p = p.Prox
	}
	return result
}

func insereEnc(L **NoEnc, c, e int, u **NoEnc) {
	novo := &NoEnc{Chave: c, E: e, Prox: nil}
	if *u == nil {
		*L = novo
	} else {
		(*u).Prox = novo
	}
	*u = novo
}

func escreveEnc(L *NoEnc) {
	fmt.Print("[")
	for p := L; p != nil; p = p.Prox {
		fmt.Printf("c=%d;e=%d ", p.Chave, p.E)
	}
	fmt.Println("]")
}

func destroi(L **NoEnc) {
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

	for {
		n := s.NextInt()
		if n != -1 {
			var L, u *NoEnc
			for i := 0; i < n; i++ {
				c := s.NextInt()
				e := s.NextInt()
				insereEnc(&L, c, e, &u)
			}
			var N int
			Lconv1 := ConverterEncParaSeq(L, &N)
			Lconv2 := ConverterSeqParaEnc(Lconv1, N)
			escreveEnc(Lconv2)
			destroi(&Lconv2)
			destroi(&L)
		} else {
			n = 500000
			var L, u *NoEnc
			for i := 0; i < n; i++ {
				insereEnc(&L, i, n+i, &u)
			}
			var N int
			Lconv1 := ConverterEncParaSeq(L, &N)
			Lconv2 := ConverterSeqParaEnc(Lconv1, N)
			escreveEnc(Lconv2)
			destroi(&Lconv2)
			destroi(&L)
		}
	}
}
