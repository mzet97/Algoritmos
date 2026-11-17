package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

func Exp(b, n int) int {
	if n == 0 {
		return 1
	}
	if n%2 == 0 {
		half := Exp(b, n/2)
		return half * half
	}
	return b * Exp(b, n-1)
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 1024*1024), 1024*1024)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		parts := strings.Fields(line)
		b := 0
		n := 0
		fmt.Sscan(parts[0], &b)
		fmt.Sscan(parts[1], &n)
		if b >= 0 {
			fmt.Println(Exp(b, n))
		} else {
			b = 1
			n = 50000
			for t := 1; t <= 200; t++ {
				Exp(b, n)
			}
			fmt.Println(Exp(b, n))
		}
	}
}
