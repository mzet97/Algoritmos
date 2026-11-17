package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

func Impar(n int) int {
	if n == 0 {
		return -1
	}
	return Impar(n-1) + 2
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
			fmt.Println(Impar(n))
		} else {
			n = 50000
			for t := 1; t <= 200; t++ {
				Impar(n)
			}
			fmt.Println(Impar(n))
		}
	}
}
