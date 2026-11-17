package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
)

func CalculaSoma(n int) int64 {
	var soma int64
	for i := 1; i <= n; i++ {
		soma += int64(i)
	}
	return soma
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
		fmt.Printf("%d\n", CalculaSoma(n))
	}
}
