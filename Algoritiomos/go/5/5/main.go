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

func Maior(T *No) int {
	if T == nil {
		return -1
	}
	m := T.E
	l := Maior(T.Esq)
	if l > m {
		m = l
	}
	r := Maior(T.Dir)
	if r > m {
		m = r
	}
	return m
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

func preencheCheiaBusca(T **No, idx *int, h int) {
	if h == 0 {
		*T = nil
	} else {
		preencheCheiaBusca(&(*T).Esq, idx, h-1)
		*T = &No{E: *idx}
		*idx++
		preencheCheiaBusca(&(*T).Dir, idx, h-1)
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
			fmt.Printf("%d\n", Maior(T))
			destroi(&T)
		} else {
			idx := 1
			preencheCheiaBusca(&T, &idx, 20)
			for t := 1; t <= 100000; t++ {
				Maior(T)
			}
			fmt.Printf("%d\n", Maior(T))
			destroi(&T)
		}
	}
}
