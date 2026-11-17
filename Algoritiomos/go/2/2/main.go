package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

type No struct {
	E    int
	Prox *No
}

func Maior(L *No) int {
	max := L.E
	for p := L.Prox; p != nil; p = p.Prox {
		if p.E > max {
			max = p.E
		}
	}
	return max
}

func insere(L **No, e int) {
	novo := &No{E: e, Prox: *L}
	*L = novo
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 1024*1024), 1024*1024)
	var L *No

	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		parts := strings.Fields(line)
		op := parts[0]
		if op[0] != 'a' {
			if op[0] == 'i' {
				e, _ := strconv.Atoi(parts[1])
				insere(&L, e)
			} else {
				fmt.Printf("%d\n", Maior(L))
			}
		} else {
			n := 1000000
			for i := n / 2; i <= n; i++ {
				insere(&L, i)
			}
			for i := 1; i < n/2; i++ {
				insere(&L, i)
			}
			fmt.Printf("%d\n", Maior(L))
		}
	}
}
