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
	Esq  *No
	Dir  *No
}

func EhArvoreBusca(T *No) int {
	if isBST(T, -1<<31, 1<<31-1) {
		return 1
	}
	return 0
}

func isBST(T *No, min, max int) bool {
	if T == nil {
		return true
	}
	if T.E <= min || T.E >= max {
		return false
	}
	return isBST(T.Esq, min, T.E) && isBST(T.Dir, T.E, max)
}

func insere(T **No, e []int, idx *int) {
	if e[*idx] == 0 {
		*idx++
		*T = nil
	} else {
		*T = &No{E: e[*idx]}
		*idx++
		insere(&(*T).Esq, e, idx)
		insere(&(*T).Dir, e, idx)
	}
}

func destroi(T **No) {
	if *T != nil {
		destroi(&(*T).Esq)
		destroi(&(*T).Dir)
		*T = nil
	}
}

func insereBal(T **No, i, j int) {
	if i <= j {
		*T = &No{}
		m := (i + j) / 2
		(*T).E = m
		if m == 900000 {
			(*T).E = 1
		}
		insereBal(&(*T).Esq, i, m-1)
		insereBal(&(*T).Dir, m+1, j)
	}
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 1024*1024), 1024*1024)
	e := make([]int, 1000000)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		e[0], _ = strconv.Atoi(line)
		var T *No
		if e[0] > -1 {
			n := 0
			for e[n] != -1 {
				n++
				scanner.Scan()
				e[n], _ = strconv.Atoi(strings.TrimSpace(scanner.Text()))
			}
			idx := 0
			insere(&T, e, &idx)
		} else {
			insereBal(&T, 1, 900000)
		}
		fmt.Printf("%d\n", EhArvoreBusca(T))
		destroi(&T)
	}
}
