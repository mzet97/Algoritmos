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
	Q    int
	Esq  *No
	Dir  *No
}

func QuantidadeIntervalo(T *No, a, b int) int {
	if T == nil {
		return 0
	}
	if T.E < a {
		return QuantidadeIntervalo(T.Dir, a, b)
	}
	if T.E > b {
		return QuantidadeIntervalo(T.Esq, a, b)
	}
	// T.E is in [a,b]
	return 1 + QuantidadeIntervalo(T.Esq, a, b) + QuantidadeIntervalo(T.Dir, a, b)
}

func destroi(T **No) {
	if *T != nil {
		destroi(&(*T).Esq)
		destroi(&(*T).Dir)
		*T = nil
	}
}

func insereBST(T **No, x int) {
	if *T == nil {
		*T = &No{E: x, Q: 1}
	} else if (*T).E > x {
		insereBST(&(*T).Esq, x)
		(*T).Q++
	} else {
		insereBST(&(*T).Dir, x)
		(*T).Q++
	}
}

func insereBal(T **No, i, j int) {
	if i <= j {
		m := (i + j) / 2
		insereBST(T, m)
		insereBal(T, i, m-1)
		insereBal(T, m+1, j)
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
		n, _ := strconv.Atoi(line)
		var T *No
		var a, b int
		if n > 0 {
			for i := 0; i < n; i++ {
				scanner.Scan()
				x, _ := strconv.Atoi(strings.TrimSpace(scanner.Text()))
				insereBST(&T, x)
			}
			scanner.Scan()
			parts := strings.Fields(strings.TrimSpace(scanner.Text()))
			a, _ = strconv.Atoi(parts[0])
			b, _ = strconv.Atoi(parts[1])
		} else {
			insereBal(&T, 1, 500000)
			a = 122111
			b = 422111
		}
		for t := 1; t <= 100000; t++ {
			QuantidadeIntervalo(T, a, b)
		}
		fmt.Printf("%d\n", QuantidadeIntervalo(T, a, b))
		destroi(&T)
	}
}
