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

func SomaFolhas(T *No) int {
	if T == nil {
		return 0
	}
	if T.Esq == nil && T.Dir == nil {
		return T.E
	}
	return SomaFolhas(T.Esq) + SomaFolhas(T.Dir)
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

func preencheCheia(T **No, h int) {
	if h == 0 {
		*T = nil
	} else {
		*T = &No{E: h}
		preencheCheia(&(*T).Esq, h-1)
		preencheCheia(&(*T).Dir, h-1)
	}
}

func preencheZigZag(T **No, h int) {
	if h == 0 {
		*T = nil
	} else {
		*T = &No{E: h}
		preencheZigZag(&(*T).Esq, h-1)
		(*T).Dir = nil
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
		if e[0] > -2 {
			n := 0
			for e[n] != -1 {
				n++
				scanner.Scan()
				e[n], _ = strconv.Atoi(strings.TrimSpace(scanner.Text()))
			}
			idx := 0
			insere(&T, e, &idx)
			fmt.Printf("%d\n", SomaFolhas(T))
			destroi(&T)
		} else {
			if e[0] == -2 {
				preencheCheia(&T, 19)
			} else {
				preencheZigZag(&T, 50000)
			}
			fmt.Printf("%d\n", SomaFolhas(T))
			destroi(&T)
		}
	}
}
