package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

type No struct {
	Op   byte
	E    int
	Esq  *No
	Dir  *No
}

func AvaliarExpressao(T *No) int {
	if T == nil {
		return 0
	}
	if T.Op == '@' {
		return T.E
	}
	l := AvaliarExpressao(T.Esq)
	r := AvaliarExpressao(T.Dir)
	switch T.Op {
	case '+':
		return l + r
	case '-':
		return l - r
	case '*':
		return l * r
	case '/':
		return l / r
	}
	return 0
}

func destroi(T **No) {
	if *T != nil {
		destroi(&(*T).Esq)
		destroi(&(*T).Dir)
		*T = nil
	}
}

func insere(T **No, e string, idx *int) {
	if e[*idx] == '.' {
		*idx++
		*T = nil
	} else {
		*T = &No{}
		if e[*idx] >= '0' && e[*idx] <= '9' {
			(*T).E = int(e[*idx] - '0')
			(*T).Op = '@'
		} else {
			(*T).Op = e[*idx]
		}
		*idx++
		insere(&(*T).Esq, e, idx)
		insere(&(*T).Dir, e, idx)
	}
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 1024*1024), 1024*1024)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		var T *No
		idx := 0
		insere(&T, line, &idx)
		fmt.Printf("%d\n", AvaliarExpressao(T))
		destroi(&T)
	}
}
