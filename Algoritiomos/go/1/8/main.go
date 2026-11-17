package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
)

func Fibonacci(n int) int {
	if n == 0 {
		return 0
	}
	if n == 1 {
		return 1
	}
	const MOD = 1000000000
	a, b := 0, 1
	for i := 2; i <= n; i++ {
		a, b = b, (a+b)%MOD
	}
	return b
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for scanner.Scan() {
		line := scanner.Text()
		if line == "" {
			continue
		}
		n, err := strconv.Atoi(line)
		if err != nil {
			continue
		}
		fmt.Printf("%d\n", Fibonacci(n))
	}
}
