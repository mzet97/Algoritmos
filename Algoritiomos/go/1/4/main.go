package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
)

func QtdePrimos(n int) int {
	if n < 2 {
		return 0
	}
	count := 0
	for i := 2; i <= n; i++ {
		primo := true
		for j := 2; j*j <= i; j++ {
			if i%j == 0 {
				primo = false
				break
			}
		}
		if primo {
			count++
		}
	}
	return count
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
		fmt.Printf("%d\n", QtdePrimos(n))
	}
}
