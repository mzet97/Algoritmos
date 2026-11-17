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
	Ant  *No
	Prox *No
}

func KUltimo(L *No, k int) int {
	if L == nil {
		return -1
	}
	p := L.Ant // last element
	for i := 1; i < k; i++ {
		p = p.Ant
		if p == L.Ant {
			return -1 // wrapped around
		}
	}
	return p.E
}

func insere(L **No, e int) {
	novo := &No{E: e}
	if *L == nil {
		novo.Prox = novo
		novo.Ant = novo
		*L = novo
	} else {
		novo.Ant = (*L).Ant
		(*L).Ant.Prox = novo
		novo.Prox = *L
		(*L).Ant = novo
		*L = novo
	}
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
				k, _ := strconv.Atoi(parts[1])
				fmt.Printf("%d\n", KUltimo(L, k))
			}
		} else {
			n := 2000000
			for i := 1; i <= n; i++ {
				insere(&L, i)
			}
			if len(parts) > 1 && parts[0] == "a1" {
				fmt.Printf("%d\n", KUltimo(L, n/2))
			} else {
				fmt.Printf("%d\n", KUltimo(L, 3))
				for i := 1; i <= 1000; i++ {
					KUltimo(L, 3)
				}
			}
		}
	}
}
