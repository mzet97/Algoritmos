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

func Ultimo(L *No) int {
	if L == nil {
		return -1
	}
	p := L
	for p.Prox != L {
		p = p.Prox
	}
	return p.E
}

func insere(L **No, e int, u **No) {
	novo := &No{E: e}
	if *L == nil {
		novo.Prox = novo
		*L = novo
	} else {
		novo.Prox = *L
		(*u).Prox = novo
	}
	*u = novo
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 1024*1024), 1024*1024)
	var L *No
	var u *No

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
				insere(&L, e, &u)
			} else {
				fmt.Printf("%d\n", Ultimo(L))
			}
		} else {
			for i := 1; i <= 2000000; i++ {
				insere(&L, i, &u)
			}
			fmt.Printf("%d\n", Ultimo(L))
		}
	}
}
