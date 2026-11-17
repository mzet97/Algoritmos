package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

func Divisores(n, k int) int {
	if k == 0 {
		return 0
	}
	cnt := 0
	if n%k == 0 {
		cnt = 1
	}
	return cnt + Divisores(n, k-1)
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
		n := 0
		k := 0
		fmt.Sscan(parts[0], &n)
		fmt.Sscan(parts[1], &k)
		if n >= 0 {
			fmt.Println(Divisores(n, k))
		} else {
			n = 50000
			k = 50000
			for t := 1; t <= 200; t++ {
				Divisores(n, k)
			}
			fmt.Println(Divisores(n, k))
		}
	}
}
