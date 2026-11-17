package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
)

func ContaMisteriosa(n int) float64 {
	var soma float64
	limite := 1
	for i := 0; i < n; i++ {
		limite *= 10
	}
	for k := 1; k <= limite; k++ {
		var sinal float64
		if (k+1)%2 == 0 {
			sinal = 1.0
		} else {
			sinal = -1.0
		}
		soma += sinal / float64(2*k-1)
	}
	return 4.0 * soma
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
		fmt.Printf("%.8f\n", ContaMisteriosa(n))
	}
}
