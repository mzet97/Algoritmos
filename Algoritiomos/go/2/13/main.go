package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

func ParesIguais(A []int, B []int, n int) int64 {
	var count int64
	i, j := 0, 0
	for i < n && j < n {
		if A[i] == B[j] {
			cntA := int64(1)
			val := A[i]
			for i+1 < n && A[i+1] == val {
				i++
				cntA++
			}
			cntB := int64(1)
			for j+1 < n && B[j+1] == val {
				j++
				cntB++
			}
			count += cntA * cntB
			i++
			j++
		} else if A[i] < B[j] {
			i++
		} else {
			j++
		}
	}
	return count
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
		maxN := s.NextInt()
		if maxN > -1 {
			A := make([]int, maxN)
			for i := 0; i < maxN; i++ {
				A[i] = s.NextInt()
			}
			B := make([]int, maxN)
			for i := 0; i < maxN; i++ {
				B[i] = s.NextInt()
			}
			fmt.Printf("%d\n", ParesIguais(A, B, maxN))
		} else {
			nlin := maxN
			maxN = 200000
			A := make([]int, maxN)
			B := make([]int, maxN)
			if nlin == -1 {
				for i := 0; i < maxN; i++ {
					A[i] = i
					B[i] = i
				}
			} else if nlin == -2 {
				for i := 0; i < maxN; i++ {
					A[i] = 1
					B[i] = 1
				}
			} else {
				idx := 0
				for i := 0; i < maxN/10000; i++ {
					for j := 0; j < 10000; j++ {
						A[idx] = i
						B[idx] = i
						idx++
					}
				}
			}
			fmt.Printf("%d\n", ParesIguais(A, B, maxN))
		}
	}
}
