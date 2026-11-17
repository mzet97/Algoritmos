package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
)

func CalculaSoma(n int) int64 {
	return int64(n) * int64(n+1) / 2
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
