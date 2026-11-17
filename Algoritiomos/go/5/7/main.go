package main

import (
	"bufio"
	"fmt"
	"os"
	"sort"
	"strconv"
	"strings"
)

type NoL struct {
	E    int
	Prox *NoL
}

type No struct {
	E    int
	Esq  *No
	Dir  *No
}

func ListaNosInternos(T *No) *NoL {
	if T == nil {
		return nil
	}
	var result *NoL
	// Internal node: has at least one child
	if T.Esq != nil || T.Dir != nil {
		childList := concat(ListaNosInternos(T.Esq), ListaNosInternos(T.Dir))
		novo := &NoL{E: T.E, Prox: childList}
		result = novo
	} else {
		result = concat(ListaNosInternos(T.Esq), ListaNosInternos(T.Dir))
	}
	return result
}

func concat(a, b *NoL) *NoL {
	if a == nil {
		return b
	}
	p := a
	for p.Prox != nil {
		p = p.Prox
	}
	p.Prox = b
	return a
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

func escreveLista(L *NoL) {
	var vals []int
	for p := L; p != nil; p = p.Prox {
		vals = append(vals, p.E)
	}
	sort.Ints(vals)
	fmt.Printf("[ ")
	for _, v := range vals {
		fmt.Printf("%d ", v)
	}
	fmt.Printf("]\n")
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
			if e[0] == ENTRADA_AUTOMATICA {
				preencheZigZag(&T, 50000)
			} else {
				preencheCheia(&T, 19)
			}
		}
		L := ListaNosInternos(T)
		escreveLista(L)
		destroi(&T)
	}
}
