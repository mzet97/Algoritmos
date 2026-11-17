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

func MaiorSubarvore(T *No) int {
	if T == nil {
		return 0
	}
	sum := T.E + MaiorSubarvore(T.Esq) + MaiorSubarvore(T.Dir)
	return sum
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
		*T = &No{}
		if h > 1 {
			(*T).E = -2
		} else {
			(*T).E = 1
		}
		preencheCheia(&(*T).Esq, h-1)
		preencheCheia(&(*T).Dir, h-1)
	}
}

func preencheZigZag(T **No, h int) {
	if h == 0 {
		*T = nil
	} else {
		*T = &No{}
		if h > 1 {
			(*T).E = -2
		} else {
			(*T).E = 1
		}
		preencheZigZag(&(*T).Esq, h-1)
		(*T).Dir = nil
	}
}

const FIM_ENTRADA = -1000
const ENTRADA_AUTOMATICA = -2000

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
		if e[0] > ENTRADA_AUTOMATICA {
			n := 0
			for e[n] != FIM_ENTRADA {
				n++
				scanner.Scan()
				e[n], _ = strconv.Atoi(strings.TrimSpace(scanner.Text()))
			}
			idx := 0
			insere(&T, e, &idx)
			fmt.Printf("%d\n", MaiorSubarvore(T))
			destroi(&T)
		} else {
			if e[0] == ENTRADA_AUTOMATICA {
				preencheZigZag(&T, 50000)
			} else {
				preencheCheia(&T, 19)
			}
			fmt.Printf("%d\n", MaiorSubarvore(T))
			destroi(&T)
		}
	}
}
