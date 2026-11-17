package main

import (
	"bufio"
	"fmt"
	"os"
	"sort"
	"strconv"
	"strings"
)

type No struct {
	Col  int
	E    int
	Prox *No
}

type MatrizEsparsa struct {
	N      int
	Linhas []*No
}

func AoQuadrado(M MatrizEsparsa) MatrizEsparsa {
	R := MatrizEsparsa{
		N:      M.N,
		Linhas: make([]*No, M.N),
	}

	for i := 0; i < M.N; i++ {
		acc := make(map[int]int)
		for pk := M.Linhas[i]; pk != nil; pk = pk.Prox {
			k := pk.Col
			for pj := M.Linhas[k]; pj != nil; pj = pj.Prox {
				acc[pj.Col] += pk.E * pj.E
			}
		}
		if len(acc) == 0 {
			continue
		}
		cols := make([]int, 0, len(acc))
		for col, val := range acc {
			if val != 0 {
				cols = append(cols, col)
			}
		}
		sort.Ints(cols)
		var head, tail *No
		for _, col := range cols {
			novo := &No{Col: col, E: acc[col]}
			if head == nil {
				head = novo
			} else {
				tail.Prox = novo
			}
			tail = novo
		}
		R.Linhas[i] = head
	}

	return R
}

func escreve(M MatrizEsparsa) {
	fmt.Print("[")
	for i := 0; i < M.N; i++ {
		fmt.Print("[")
		for p := M.Linhas[i]; p != nil; p = p.Prox {
			fmt.Printf("c=%d;e=%d ", p.Col, p.E)
		}
		fmt.Print("]")
	}
	fmt.Println("]")
}

func addFim(row **No, tail **No, col, e int) {
	novo := &No{Col: col, E: e}
	if *tail == nil {
		*row = novo
	} else {
		(*tail).Prox = novo
	}
	*tail = novo
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 1024), 1024*1024)

	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		parts := strings.Fields(line)
		if len(parts) < 2 {
			continue
		}
		n, _ := strconv.Atoi(parts[0])
		k, _ := strconv.Atoi(parts[1])

		var M MatrizEsparsa
		if n != -1 {
			M = MatrizEsparsa{N: n, Linhas: make([]*No, n)}
			tails := make([]*No, n)
			for i := 0; i < k; i++ {
				scanner.Scan()
				vals := strings.Fields(strings.TrimSpace(scanner.Text()))
				l, _ := strconv.Atoi(vals[0])
				c, _ := strconv.Atoi(vals[1])
				e, _ := strconv.Atoi(vals[2])
				addFim(&M.Linhas[l], &tails[l], c, e)
			}
		} else {
			M = MatrizEsparsa{N: 1000, Linhas: make([]*No, 1000)}
			tails := make([]*No, 1000)
			for l := 0; l < 10; l++ {
				for c := 0; c < 10; c++ {
					addFim(&M.Linhas[l], &tails[l], c, 1)
				}
			}
		}

		escreve(AoQuadrado(M))
	}
}
