package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

func EhPalindromo(C string, n int) int {
	return palAux(C, 0, n-1)
}

func palAux(C string, lo, hi int) int {
	if lo >= hi {
		return 1
	}
	if C[lo] != C[hi] {
		return 0
	}
	return palAux(C, lo+1, hi-1)
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 1024*1024), 1024*1024)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		n, _ := strconv.Atoi(line)
		if n >= 0 {
			scanner.Scan()
			C := strings.TrimSpace(scanner.Text())
			fmt.Println(EhPalindromo(C, n))
		} else {
			nlin := n
			n = 50000
			C := make([]byte, n)
			if nlin == -1 {
				for i := 0; i < n; i++ {
					C[i] = byte('a' + (i % 26))
				}
				fmt.Println(EhPalindromo(string(C), n))
			} else {
				for i := 0; i < (n+1)/2; i++ {
					C[i] = byte('a' + (i % 26))
					C[n-i-1] = C[i]
				}
				fmt.Println(EhPalindromo(string(C), n))
			}
		}
	}
}
