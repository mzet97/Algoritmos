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
	fast := L
	for i := 0; i < k; i++ {
		fast = fast.Prox
	}
	slow := L
	for fast != nil {
		fast = fast.Prox
		slow = slow.Prox
	}
	return slow.E
}

func Constroi(L **No, u **No) {
	*L = nil
	*u = nil
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
	Constroi(&L, &u)
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
				k, _ := strconv.Atoi(parts[1])
				fmt.Printf("%d\n", KUltimo(L, k))
			}
		} else {
			n := 50000
			for i := 1; i <= n; i++ {
				Insere(&L, i, &u)
			}
			for t := 1; t <= 100; t++ {
				KUltimo(L, n/2)
			}
			fmt.Printf("%d\n", KUltimo(L, n/2))
		}
	}
}
