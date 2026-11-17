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
	Niv  int
	Esq  *No
	Dir  *No
}

func AtualizaNivel(T *No, nivel int) {
	if T == nil {
		return
	}
	T.Niv = nivel
	AtualizaNivel(T.Esq, nivel+1)
	AtualizaNivel(T.Dir, nivel+1)
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

func preencheCheia(T **No, h int, idx *int) {
	if h == 0 {
		*T = nil
	} else {
		*T = &No{E: *idx}
		*idx++
		preencheCheia(&(*T).Esq, h-1, idx)
		preencheCheia(&(*T).Dir, h-1, idx)
	}
}

func preencheZigZag(T **No, h int, idx *int) {
	if h == 0 {
		*T = nil
	} else {
		*T = &No{E: *idx}
		*idx++
		preencheZigZag(&(*T).Esq, h-1, idx)
		(*T).Dir = nil
	}
}

func escreve(T *No) {
	if T != nil {
		fmt.Printf("niv(%d)=%d ", T.E, T.Niv)
		escreve(T.Esq)
		escreve(T.Dir)
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
		} else {
			idx := 1
			if e[0] == ENTRADA_AUTOMATICA {
				preencheCheia(&T, 19, &idx)
			} else {
				preencheZigZag(&T, 50000, &idx)
			}
		}
		AtualizaNivel(T, 0)
		escreve(T)
		fmt.Println()
		destroi(&T)
	}
}
