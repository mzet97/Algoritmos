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

func KUltimo(L *No, k int) int {
	// Count elements (skip head node)
	n := 0
	p := L.Prox
	for p != L {
		n++
		p = p.Prox
	}
	if k < 1 || k > n {
		return -1
	}
	// Walk to the (n-k+1)-th element from head
	target := n - k + 1
	p = L.Prox
	for i := 1; i < target; i++ {
		p = p.Prox
	}
	return p.E
}

func constroi(L **No, u **No) {
	*L = &No{E: 0}
	*u = *L
	(*L).Prox = *L
}

func insere(L *No, e int, u **No) {
	novo := &No{E: e}
	novo.Prox = (*u).Prox
	(*u).Prox = novo
	*u = novo
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 1024*1024), 1024*1024)
	var L *No
	var u *No
	constroi(&L, &u)

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
				insere(L, e, &u)
			} else {
				k, _ := strconv.Atoi(parts[1])
				fmt.Printf("%d\n", KUltimo(L, k))
			}
		} else {
			n := 2000000
			for i := 1; i <= n; i++ {
				insere(L, i, &u)
			}
			fmt.Printf("%d\n", KUltimo(L, n/2))
		}
	}
}
