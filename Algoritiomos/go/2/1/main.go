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

func NumeroElementos(L *No) int {
	count := 0
	for p := L; p != nil; p = p.Prox {
		count++
	}
	return count
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
				fmt.Printf("%d\n", NumeroElementos(L))
			}
		} else {
			for i := 1; i <= 1000000; i++ {
				insere(&L, i)
			}
			fmt.Printf("%d\n", NumeroElementos(L))
		}
	}
}
