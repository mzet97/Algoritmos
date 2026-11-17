package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
)

func CalculaSoma(n int, m []int, k int) int64 {
	var soma int64
	for i := 1; i <= n; i++ {
		for j := 0; j < k; j++ {
			if i%m[j] == 0 {
				soma += int64(i)
				break
			}
		}
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
		var n, k int
		_, err := fmt.Sscanf(line, "%d %d", &n, &k)
		if err != nil {
			continue
		}
		m := make([]int, k)
		for i := 0; i < k; i++ {
			scanner.Scan()
			m[i], _ = strconv.Atoi(scanner.Text())
		}
		fmt.Printf("%d\n", CalculaSoma(n, m, k))
	}
}
