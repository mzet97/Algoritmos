package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

func Bin(n, k int) int64 {
	if k == 0 || k == n {
		return 1
	}
	return int64(n) * Bin(n-1, k-1) / int64(k)
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
			fmt.Println(Bin(n, k))
		} else {
			n = 50000
			k = 49999
			for t := 1; t <= 200; t++ {
				Bin(n, k)
			}
			fmt.Println(Bin(n, k))
		}
	}
}
