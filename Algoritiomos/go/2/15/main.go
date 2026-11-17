package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

func Busca(B [][]int, n int, x int) bool {
	i := 0
	j := n - 1
	for i < n && j >= 0 {
		if B[i][j] == x {
			return true
		} else if B[i][j] > x {
			j--
		} else {
			i++
		}
	}
	return false
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
		if n > -1 {
			B := make([][]int, n)
			for i := 0; i < n; i++ {
				B[i] = make([]int, n)
				for j := 0; j < n; j++ {
					B[i][j] = s.NextInt()
				}
			}
			x := s.NextInt()
			if Busca(B, n, x) {
				fmt.Println(1)
			} else {
				fmt.Println(0)
			}
		} else {
			nlin := n
			n = 16000
			B := make([][]int, n)
			for i := 0; i < n; i++ {
				B[i] = make([]int, n)
			}
			if nlin == -1 || nlin == -2 {
				for i := 0; i < n; i++ {
					for j := 0; j < n; j++ {
						B[i][j] = i
					}
				}
				B[n-1][n-2] = n
				B[n-1][n-1] = n + 1
				if nlin == -1 {
					if Busca(B, n, n) {
						fmt.Println(1)
					} else {
						fmt.Println(0)
					}
				} else {
					if Busca(B, n, n+2) {
						fmt.Println(1)
					} else {
						fmt.Println(0)
					}
				}
			} else if nlin == -3 || nlin == -4 {
				for i := 0; i < n; i++ {
					for j := 0; j < n; j++ {
						B[i][j] = j
					}
				}
				B[n-1][n-2] = n
				B[n-1][n-1] = n + 1
				if nlin == -3 {
					if Busca(B, n, n) {
						fmt.Println(1)
					} else {
						fmt.Println(0)
					}
				} else {
					if Busca(B, n, n+2) {
						fmt.Println(1)
					} else {
						fmt.Println(0)
					}
				}
			} else {
				for i := 0; i < n; i++ {
					for j := 0; j < n; j++ {
						B[i][j] = 3
					}
				}
				B[0][1] = 2
				B[0][0] = 1
				if nlin == -5 {
					if Busca(B, n, 2) {
						fmt.Println(1)
					} else {
						fmt.Println(0)
					}
				} else {
					if Busca(B, n, 0) {
						fmt.Println(1)
					} else {
						fmt.Println(0)
					}
					for i := 0; i < 1000; i++ {
						Busca(B, n, 0)
					}
				}
			}
		}
	}
}
