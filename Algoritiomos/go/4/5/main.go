package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

func RaizQuad(n int) int {
	return raizQuadAux(n, 0)
}

func raizQuadAux(n, r int) int {
	if (r+1)*(r+1) > n {
		return r
	}
	return raizQuadAux(n, r+1)
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
