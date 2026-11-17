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
	for L.Prox != nil {
		L = L.Prox
	}
	return L.E
}

func Insere(L **No, e int, u **No) {
	novo := &No{E: e, Prox: nil}
	if *L == nil {
		*L = novo
	} else {
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
				Insere(&L, e, &u)
			} else {
				fmt.Printf("%d\n", Ultimo(L))
			}
		} else {
			for i := 1; i <= 50000; i++ {
				Insere(&L, i, &u)
			}
			for t := 1; t <= 100; t++ {
				Ultimo(L)
			}
			fmt.Printf("%d\n", Ultimo(L))
		}
	}
}
