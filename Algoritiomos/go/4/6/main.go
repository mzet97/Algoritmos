package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

func RaizQuad(n int) int {
	return raizQuadAux(n, 0, n)
}

func raizQuadAux(n, lo, hi int) int {
	if lo > hi {
		return hi
	}
	mid := (lo + hi) / 2
	sq := mid * mid
	if sq == n {
		return mid
	}
	if sq < n {
		if (mid+1)*(mid+1) > n {
			return mid
		}
		return raizQuadAux(n, mid+1, hi)
	}
	return raizQuadAux(n, lo, mid-1)
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
			fmt.Println(RaizQuad(n))
		} else {
			n = 50000
			for t := 1; t <= 200; t++ {
				RaizQuad(n)
			}
			fmt.Println(RaizQuad(n))
		}
	}
}
