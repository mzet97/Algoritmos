package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

func NumeroCaminhos(n int, m int, X []int, Y []int) int64 {
	// Build adjacency list representation
	adj := make([][]int, n+1)
	for i := 0; i < m; i++ {
		adj[X[i]] = append(adj[X[i]], Y[i])
	}
	// Count paths from node 1 to node n using DP
	// dp[i] = number of paths from 1 to i
	dp := make([]int64, n+1)
	dp[1] = 1
	for i := 1; i <= n; i++ {
		if dp[i] > 0 {
			for _, v := range adj[i] {
				dp[v] += dp[i]
			}
		}
	}
	return dp[n]
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
		n, _ := strconv.Atoi(parts[0])
		m, _ := strconv.Atoi(parts[1])
		if n >= 0 {
			X := make([]int, m)
			Y := make([]int, m)
			// Read X
			for i := 0; i < m; i++ {
				scanner.Scan()
				X[i], _ = strconv.Atoi(strings.TrimSpace(scanner.Text()))
			}
			// Read Y
			for i := 0; i < m; i++ {
				scanner.Scan()
				Y[i], _ = strconv.Atoi(strings.TrimSpace(scanner.Text()))
			}
			fmt.Printf("%d\n", NumeroCaminhos(n, m, X, Y))
		} else {
			// automatic test
			n = 2000
			m = 2 * n
			X := make([]int, m)
			Y := make([]int, m)
			m = 0
			for i := 2; i <= n; i++ {
				X[m] = i
				Y[m] = i - 1
				m++
				X[m] = i - 1
				Y[m] = i + 1
				m++
			}
			fmt.Printf("%d\n", NumeroCaminhos(n, m, X, Y))
		}
	}
}
